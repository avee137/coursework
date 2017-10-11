-module(tut15_ping_pong).

-export([start/0, ping/2, pong/0]).

ping(0, Pong_PID) ->
    Pong_PID ! finished, % ! is like a pipe in erlang, here atom "finished" is sent to process with Pong_PID i.e process pong
    io:format("ping finished~n", []);

ping(N, Pong_PID) ->
    % self() gives the pid of the executing process, here we send a tuple of atom "ping" and pid of process "ping"
    % to process with PID Pong_PID i.e process pong
    Pong_PID ! {ping, self()}, 
    receive % receive construct receives a message from its queue.
        pong -> % if the received message is atom pong it prints as follows
            io:format("Ping received pong~n", [])
    end,
    % ping is called again by decrementing variable N. eventually it ll be called with args ( 0, Pong_PID) and above
    % construct of function ping i.e ping (0, Pong_PID) will be called. 
    ping(N - 1, Pong_PID).


pong() ->
    receive % receives a message from process ping
        finished -> % if it receives atom received which is sent when ping is called eventually like ping (0, Pong_PID)
            io:format("Pong finished~n", []);
        {ping, Ping_PID} -> % if it receives a tuple with atom "ping" and Ping_PID 
            io:format("Pong received ping~n", []),
            Ping_PID ! pong, % send atom "pong" to process with PID Ping_PID i.e process ping
            pong() % call itself
    end.

start() ->
    %% when spawn is used , first arg is module name to identify the process uniquely, followed by function name and arg list

    Pong_PID = spawn(tut15_ping_pong, pong, []), % spawn returns its PID 
    spawn(tut15_ping_pong, ping, [3, Pong_PID]). % ping is spawned
