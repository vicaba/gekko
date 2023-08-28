# Gekko

## Description

A project for the subject of Operating Systems at the University of La Salle Barcelona

In this project I tried to adopt an Object Oriented structure. Also, to develop the client-server architecture with the server handling multiple connections at once (one thread per connection) I tried to follow the Actor Model. 

I followed some aspects of the actor model (e.g., passing messages between threads/actors on the server side using pipes). I did not create a scheduler to assign threads to actors, and mutual exclusion is still used, although the actor-pipe approach proved to take away a lot of complexity.

The thread on the server side have an epoll that collects all possible events that can be sent to the "Actor".
* A pipe for internal comunication.
* A socket to communicate over the network with the client.

I also used a GNU macro I found somewhere on the internet that let me declare closures:
```c
#define lambda(return_type, function_body) \
({ \
      return_type __fn__ function_body \
          __fn__; \
})
```

This allowed me to write something like:
```c
llist_foreach(llist,
lambda(int, (void *elem) {
	ActorDataS *actor = (ActorDataS *)elem;
	rwbytes = write(adatas_getPipe(actor)[1], frame, sizeof(Frame));
	if (rwbytes <= 0) {
		msgErrf(buffero, "An error ocurred while finishing program, exiting now\n");
		exit(EXIT_FAILURE);
	}
	return 0;
})
			       );
```

Passing a function as an argument to the llist_foreach function, that takes the llist (object-oriented) and a callback function to execute on ever y item in the list.

## The makefile

Also, although it was very time consuming, I managed to implement a Makefile that solved all dependencies given a .c file . All those dependencies must be inside the src folder and they can't contain whitespaces in ther names. The command is as follows:
```
shell gcc -MM ./$(N_DIR)/$(RAW_N).c | sed s/$(RAW_N).o\:\ //g | sed  s/\\.h/\\.c/g | sed s/\ \ \ \ //g | sed ':a;N;$!ba;s/\n/ /g' | sed s/\ \ //g | sed 's/\ \\//g' | tr " " "\n" | while read -r line; do readlink -f $$line; done | sort | uniq | sed  s/\ /\\\\\ /g | tr "\n" " "))
```

This command may NOT work depending on the versions of programs used. It was never meant to be portable anyway.

Utils in the Geeko/src/util folder are DocBlock commented but the other source files of the project aren't. That's because only utilities are meant to be useful, the other source files are specific to the project and can't be reused.

## Compiling and executing

* To compile: ```cd``` in the Geeko/src folder and use ```make```.
* Execution of this program is not possible as I don't have the source code of some binaries provided by the University.

