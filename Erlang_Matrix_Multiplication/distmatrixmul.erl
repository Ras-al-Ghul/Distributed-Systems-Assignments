%% Multiplies two matrices. Usage example:
%% $ distmatrixmul:multiply([[1,2,3],[2,3,4]], [[2,2],[1,1],[6,2]]).
%% $ distmatrixmul:multiply([[1,2,3,4],[2,3,4,3]], [[2,2],[1,1],[6,2],[11,5]]).
%% If the dimentions are incompatible, an error is thrown.

-module(distmatrixmul).
-export([multiply/2]).

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% base case of transpose
transpose([[]|_]) ->
	[];
%% transpose main, concatenates each row recursively
transpose(B) ->
  [lists:map(fun(A) -> hd(A) end, B) | transpose(lists:map(fun(A) -> tl(A) end, B))].

multiply_elements(Pair, Sum) ->
	X = element(1, Pair),   % gets first element
	Y = element(2, Pair),   % gets second element
	X * Y + Sum.            % Sum is accumulator. So product is added to Sum, 
							% and result is stored back in Sum (accumulator)

%% A and B are two single rows (one dimensional vectors).
%% foldl takes a function as first argument, with two parameters.
%% second argument is called accumulator, initialized to zero in this case
%% this accumulator is the second parameter to the first function.
%% the third argument is the list, whose elements become the first parameter 
%% of the first function. The first function is executed repeatedly over all 
%% elements of the list. The final value of the accumulator is returned as the
%% result of foldl
dot_product(A, B) ->
	lists:foldl(fun(X,Y)->multiply_elements(X,Y) end, 0, lists:zip(A, B)).

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
forkjoin(NodeList, FunctionList) when is_list(FunctionList) ->
	% When trap_exit is set to true, exit signals arriving 
	% to a process are converted to {'EXIT', From, Reason} messages
	process_flag(trap_exit, true),

	% self returns own PID
	ParentPid = self(),

	% fork
	% spawn_link - spawns and links to current process atomically
	PidList =   [spawn_link(NodeName, 
					fun () -> 
						% executes Fun(), which is basically the dot product execution
						% this entire tuple is sent to Parent process
						ParentPid ! {'RESULT', self(), Fun()}
					end)
					% fun sendto/2, [ParentPid, Fun])
					% spawn_link is executed for each function in the functionlist
					|| {Fun, NodeName} <- lists:zip(FunctionList, NodeList)
				],
	% join
	% receives messages from spawned processes
	[   receive
			% computed result
			{'RESULT', Pid, Result} ->
				% Result;
				Result
			% error case, if spawned process exits erroneously
			% {'EXIT', Pid, Reason} ->
			% 	erlang:display(Reason),
			% 	[]
		end 
		% keeps receiving for each spawned process, stored in PidList
	|| Pid <- PidList].

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
multiply(A, B, NodeList) when not is_list(hd(A)) and is_list(hd(B)) ->
	% distribute computation of dot product of two rows
	forkjoin(NodeList, [fun() -> dot_product(A,X) end || X<-B]).

multiply(A, B) when is_list(hd(A)) and is_list(hd(B)) -> 
	% first find transpose of B
	BT = transpose(B),
	
	% find number of rows in transposed B
	RowCount = length(BT),
	
	% spawn those many nodes to distribute processing
	NodeNames = [list_to_atom(atom_to_list(node) ++ integer_to_list(NodeNumber)) 
				|| NodeNumber <- lists:seq(1, RowCount)],
	Hostname = element(2,inet:gethostname()),
	NodeList = [element(2, Pair) || Pair <-[slave:start_link(Hostname, Node) || Node <- NodeNames]],
	
	% then call multiply for each row in A
	Result = [multiply(X,BT, NodeList) || X<-A],
	
	% stop all the nodes
	[slave:stop(NodeName) || NodeName <- NodeList],
	
	io:format("~w~n", [Result]).