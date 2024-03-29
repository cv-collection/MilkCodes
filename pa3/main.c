/**
 * @file     main.c
 * @Author   @seniorkot
 * @date     May, 2018
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

#include "log3pa.h"
#include "communication.h"
#include "banking.h"
#include "ltime.h"

int get_proc_count(int argc, char** argv);
balance_t get_proc_balance(local_id proc_id, char** argv);

int do_parent_work(PipesCommunication* comm);
int do_child_work(PipesCommunication* comm);

int do_transfer(PipesCommunication* comm, Message* msg, BalanceState* state, BalanceHistory* history);
void update_history(BalanceState* state, BalanceHistory* history, balance_t amount, timestamp_t timestamp_msg, char inc, char fix);

/**
 * @return -1 on invalid arguments, -2 on fork error, 0 on success
 */
int main(int argc, char** argv){
	size_t i;
	int proc_count;
	int* pipes;
	pid_t* children;
	pid_t fork_id;
	local_id current_proc_id;
	PipesCommunication* comm;
	
	/* Check args */
	if (argc < 4 || (proc_count = get_proc_count(argc, argv)) == -1){
		fprintf(stderr, "Usage: %s -p X y1 y2 ... yX\n", argv[0]);
		return -1;
	}
	
	/* Initialize log files */
	log_init();
	
	/* Allocate memory for children */
	children = malloc(sizeof(pid_t) * proc_count);
	
	/* Open pipes for all processes */
	pipes = pipes_init(proc_count + 1);
	
	/* Create children processes */
	for (i = 0; i < proc_count; i++){
		fork_id = fork();
		if (fork_id < 0){
			return -2;
		}
		else if (!fork_id){
			free(children);
			break;
		}
		children[i] = fork_id;
	}
	
	/* Set current process id */
	if (!fork_id){
		current_proc_id = i + 1;
	}
	else{
		current_proc_id = PARENT_ID;
	}
	
	/* Set pipe fds to process params */
	comm = communication_init(pipes, proc_count + 1, current_proc_id, get_proc_balance(current_proc_id, argv));
	log_pipes(comm);
	
	/* Do process work */
	if (current_proc_id == PARENT_ID){
		do_parent_work(comm);
	}
	else{
		do_child_work(comm);
	}
	
	/* Waiting for all children if parent process */
	if (current_proc_id == PARENT_ID){
		for (i = 0; i < proc_count; i++){
			waitpid(children[i], NULL, 0);
		}
	}
	
	/* Finish work */
	log_destroy();
	communication_destroy(comm);
	return 0;
}

/** Do parent process work (Receive messages, payload, print history)
 *
 * @param comm		Pointer to PipesCommunication
 *
 * @return -1 on incorrect message type, 0 on success.
 */
int do_parent_work(PipesCommunication* comm){
	AllHistory all_history;
	local_id i;
	
	all_history.s_history_len = comm->total_ids - 1;
	
    receive_all_msgs(comm, STARTED);

    /* Payload */
    bank_robbery(comm, comm->total_ids - 1);

	/* Payload ended, stop children work */
	increment_lamport_time();
    send_all_stop_msg(comm);
    receive_all_msgs(comm, DONE);
	
	/* Fill in History */
	for (i = 1; i < comm->total_ids; i++){
		BalanceHistory balance_history;
		Message msg;
		
		while(receive(comm, i, &msg));
		
		if (msg.s_header.s_type != BALANCE_HISTORY){
			return -1;
		}
		
		memcpy((void*)&balance_history, msg.s_payload, sizeof(char) * msg.s_header.s_payload_len);
		all_history.s_history[i - 1] = balance_history;
	}
	
	print_history(&all_history);
	return 0;
}

/** Do child process work
 *
 * @param comm		Pointer to PipesCommunication
 *
 * @return -1 on incorrect message type, 0 on success.
 */
int do_child_work(PipesCommunication* comm){
	BalanceState balance_state;
    BalanceHistory balance_history;
	size_t done_left = comm->total_ids - 2;
	int not_stopped = 1;

    balance_history.s_id = comm->current_id;
	
	balance_state.s_balance = comm->balance;
    balance_state.s_balance_pending_in = 0;
    balance_state.s_time = 0;
	
	update_history(&balance_state, &balance_history, 0, 0, 0, 0);
	
	/* Send & receive STARTED message */
	increment_lamport_time();
	send_all_proc_event_msg(comm, STARTED);
	increment_lamport_time();
    receive_all_msgs(comm, STARTED);
	
	/* Receive TRANSFER, STOP or DONE messages */
	while(done_left || not_stopped){
		Message msg;
		
        while (receive_any(comm, &msg));
		
		if (msg.s_header.s_type == TRANSFER){
			do_transfer(comm, &msg, &balance_state, &balance_history);
		}
		else if (msg.s_header.s_type == STOP){
			update_history(&balance_state, &balance_history, 0, msg.s_header.s_local_time, 1, 0);
			send_all_proc_event_msg(comm, DONE);
			not_stopped = 0;
		}
		else if (msg.s_header.s_type == DONE){
			update_history(&balance_state, &balance_history, 0, msg.s_header.s_local_time, 1, 0);
			done_left--;
		}
		else{
			return -1;
		}
	}
	
	log_received_all_done(comm->current_id);
	
	/* Update history and send to PARENT */
	update_history(&balance_state, &balance_history, 0, 0, 1, 0);
	send_balance_history(comm, PARENT_ID, &balance_history);
	return 0;
}

/** Do transfer when message received
 *
 * @param comm		Pointer to PipesCommunication
 * @param msg		Received message
 * @param state		Balance state
 * @param history	Balance history
 *
 * @return -1 on incorrect address, -2 on sending msg error, 0 on success.
 */
int do_transfer(PipesCommunication* comm, Message* msg, BalanceState* state, BalanceHistory* history){
	TransferOrder order;
	memcpy(&order, msg->s_payload, sizeof(char) * msg->s_header.s_payload_len);
	
	update_history(state, history, 0, 0, 1, 0);
	
	/* Transfer request */
	if (comm->current_id == order.s_src){
		update_history(state, history, -order.s_amount, msg->s_header.s_local_time, 1, 0);
		update_history(state, history, 0, 0, 1, 0);
		send_transfer_msg(comm, order.s_dst, &order);
		comm->balance -= order.s_amount;
	}
	/* Transfer income */
	else if (comm->current_id == order.s_dst){
		update_history(state, history, order.s_amount, msg->s_header.s_local_time, 1, 1);
		increment_lamport_time();
		send_ack_msg(comm, PARENT_ID);
		comm->balance += order.s_amount;
	}
	else{
		return -1;
	}
	return 0;
}

/** Update process Balance history
 *
 * @param state				Balance state
 * @param history			Balance history
 * @param amount			Balance changes amount
 * @param timestamp_msg		Timestamp from message
 * @param inc				Increment flag
 * @param fix				Fix flag
 */
void update_history(BalanceState* state, BalanceHistory* history, balance_t amount, timestamp_t timestamp_msg, char inc, char fix){
	static timestamp_t prev_time = 0;
    timestamp_t curr_time = get_lamport_time() < timestamp_msg ? timestamp_msg : get_lamport_time();
	timestamp_t i;
	
	if (inc){
		curr_time++;
	}
	set_lamport_time(curr_time);
	if (fix){
		timestamp_msg--;
	}

    history->s_history_len = curr_time + 1;
	
	for (i = prev_time; i < curr_time; i++){
		state->s_time = i;
		history->s_history[i] = *state;
	}
	
	if (amount > 0){
		for (i = timestamp_msg; i < curr_time; i++){
			history->s_history[i].s_balance_pending_in += amount;
		}
	}
	
	prev_time = curr_time;
	state->s_time = curr_time;
	state->s_balance += amount;
	history->s_history[curr_time] = *state;
}

/** Get process count from command line arguments.
 *
 * @param argc		Arguments count
 * @param argv		Double char array containing command line arguments.
 *
 * @return -1 on error, any other values on success.
 */
int get_proc_count(int argc, char** argv){
	int proc_count;
	if (!strcmp(argv[1], "-p") && (proc_count = atoi(argv[2])) == (argc - 3)){
		return proc_count;
	}
	return -1;
}

/** Get process balance from command line arguments.
 *
 * @param proc_id	Process local id
 * @param argv		Double char array containing command line arguments.
 *
 * @return process balance
 */
balance_t get_proc_balance(local_id proc_id, char** argv){
	return atoi(argv[proc_id + 2]);
}
