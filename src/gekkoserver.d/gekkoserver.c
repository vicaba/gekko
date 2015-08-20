/**
 * @author: Victor Caballero (vicaba)
 */
#include <regex.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>

#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <signal.h>

#include <pthread.h>

#include <sys/epoll.h>

#include "../util/fdutil/fdutil.h"
#include "../util/FP/FP.h"
#include "../util/LinkedList/LinkedList.h"
#include "../util/regex/regex.h"
#include "../util/cLinkedList/cLinkedList.h"
#include "../util/cInteger/cInteger.h"
#include "../util/socket/socket.h"
#include "../util/thread/thread.h"

#include "./ActorDataS/ActorDataS.h"
#include "../Gekko/Gekko.h"
#include "../IbexValue/IbexValue.h"
#include "../Frame/Frame.h"

#include "../TickerSell/TickerSell.h"

#include "server_frames.h"


#define STANDARD_IBVALS	35

#define ACTOR_MAXEVENTS 15

static Gekko *gekko = NULL;

static cLinkedList *adataList = NULL;

static cLinkedList *tickerList = NULL;

static stat_sock_t sockcli_tumblingDice;

static stat_sock_t sockserv_dozers;

static stat_thread_t pth_RIV;

static cInteger *finishProgram = NULL;

/**
 * Initialize actor data list
 * @return 1 if the operation was successful
 */
int initAdataList();

/**
 * Destroys actor data list
 */
void destroyAdataList();

/**
 * Initialize Ticker list
 * @return 1 if the operation was successful
 */

int initTickerList();

/**
 * Destroys Ticker list
 */
void destroyTickerList();

/**
 * Interrupt configuration
 */
void intConf();

/**
 * Read configuration file
 * @param  filename a string representing the filename
 * @return 1 if the operation was successful
 */
int readConfigFile(char *filename);

/**
 * Read ibex values file
 * @param  filename a string representing the filename
 * @return 1 if the operation was successful
 */
int readIbexFile(char *filename);

/**
 * Connect to Tumbling Dice if not connected
 * @param  nRetrys the number of retries before returning an error if a connection can't be stablished
 * @return 0 if the operation was successful, -1 if not
 */
int connectToTumblingDiceIfNotConnected(int nRetrys);

/**
 * Disconnect from Tumbling Dice if a connection was made
 */
int disconnectFromTumblingDiceIfConnected();

/**
 * Perform last configurations on the server
 */
void configServer();

/**
 * Start listening on the server
 * @return 0 if an error ocurrs
 */
int listenOnServer();

/**
 * [stopServingIfServing description]
 */
int stopServingIfServing();

/**
 * Thread to refresh bex values periodically
 */
void* pth_refreshIbexValues(void* period);

/**
 * Function that contains the logic of updating the ibex values list
 */
int refreshIbexValues(void);

/**
 * Signal handler
 * @param the number of the signal
 */
void ksighandler(int sig);

/**
 * Finishes threads if created
 */
void finishThreadsIfCreated();

/**
 * Exit program routine
 */
void exitProgramR();

/**
 * Responsible for calling the exit program routine and exiting the program
 * @param exitStatus the exit status
 */
void exitProgram(int exitStatus);

ssize_t msgErrf(char *buffer, char *format, ...);

ssize_t msgStdof(char *buffer, char *format, ...);

/**
 * The Actor thread
 * @param the Actor retrieved from the actor list
 */
void* thread_ActorS(void *arg);

int main(void)
{
	char buffero[512];
	char *bufferi;
	int exitMenu;
	int err;
	int s;

	gek_new((void **)&gekko);
	cint_new((void **)&finishProgram);

	sockcli_tumblingDice.status = SOCK_DISCONNECTED;
	sockserv_dozers.status = SOCK_DISCONNECTED;
	pth_RIV.status = THREAD_NOT_RUNNING;

	exitMenu = 0;

	bufferi = (char *)malloc(sizeof(char) * 1);

	// Configure interrupts
	intConf();

	err = 0;
	err += readConfigFile("config.dat");
	err += readIbexFile("ibex.dat");
	if (err) {
		msgErrf(buffero, "Could not read configuration files\n");
		exitProgram(EXIT_FAILURE);
	}
	err = connectToTumblingDiceIfNotConnected(5);
	if (err != 0) {
		msgErrf(buffero, "Exiting now\n");
		exitProgram(EXIT_FAILURE);
	}

	// Once this server is connected to the Tumbling Dice server, start retrieving ibex list every X seconds
	// where X is the time read in the configuration file
	s = pthread_create(&pth_RIV.id, NULL, pth_refreshIbexValues, NULL);
	if (s != 0) {
		msgErrf(buffero, "Error creating thread RIV\n");
		pth_RIV.status = THREAD_NOT_RUNNING;
		exitProgram(EXIT_FAILURE);
	}
	pth_RIV.status = THREAD_RUNNING;

	// Perform last configurations for the listening port (server)
	configServer();
	// Start listening
	if (listenOnServer() != 0)
	{
		msgErrf(buffero, "Error while listening, exiting now\n");
		exitProgram(EXIT_FAILURE);
	}

	// WARNING: Dead code, listenOnServer call is blocking
	do {
		fdut_getsStdi(&bufferi);
		bufferi[strlen(bufferi) - 1] = '\0';

		if (strcmp(bufferi, "exit") == 0) {
			free(bufferi);
			exitProgram(EXIT_SUCCESS);
		}
	} while (1);

	return 0;
}

int initAdataList()
{
	cllist_new((void **)&adataList);
	cllist_setElemFuncs(
	    adataList,
	    adatas_new,
	    adatas_cpy,
	    NULL,
	    adatas_destroy
	);

	return 1;
}

void destroyAdataList()
{
	Frame *frame = frame_apply(GEK_FRAME_SOURCE, GEK_FRAME_TYPE_SHUTDOWN, "-");
	ssize_t rwbytes;
	char buffero[512];
	if (adataList != NULL) {
		cllist_rsync(adataList,
		             lambda (int, (LinkedList * llist)
		{
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
			return 0;

		})
		            );
		cllist_destroy(adataList);
		frame_destroy(frame);
	}
	msgStdof(buffero, "Actor data list destroy end\n");
}

int initTickerList()
{
	cllist_new((void **)&tickerList);
	cllist_setElemFuncs(
	    tickerList,
	    ts_new,
	    ts_cpy,
	    NULL,
	    ts_destroy
	);
	int i = 0;
	cLinkedList *ibexlist = gek_getIbexValues(gekko);
	int size = cllist_size(ibexlist);
	IbVal *ibval;
	TickerSell *ts;
	ts_new((void **)&ts);
	for (i = 0; i < size; i++)
	{
		ibval = cllist_getElementByIndex(ibexlist, i);
		ts_setTicker(ts, ibval_getTicker(ibval));
		cllist_append(tickerList, ts);
	}
	ts_destroy(ts);
	return 1;
}

void destroyTickerList()
{
	if (tickerList != NULL) cllist_destroy(tickerList);
}


// Configure gekko as server
void configServer()
{
	gek_getServerConfig(gekko)->sin_port = htons(8101);
	gek_getServerConfig(gekko)->sin_family = AF_INET;
}

int listenOnServer()
{
	char buffero[512];
	struct sockaddr_in cliaddr;
	int clilen;
	int sockcli;
	ssize_t rwbytes;
	int optval;


	clilen = sizeof(struct sockaddr_in);
	bzero((char *) &cliaddr, sizeof(struct sockaddr_in));


	sockserv_dozers.fd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockserv_dozers.fd < 0) {
		return 1;
	}
	// Set socket reusable address see http://serverfault.com/questions/329845/how-to-forcibly-close-a-socket-in-time-wait
	optval = 1;
	setsockopt(sockserv_dozers.fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

	if (bind(sockserv_dozers.fd, (struct sockaddr *) gek_getServerConfig(gekko), sizeof(struct sockaddr_in)) < 0) {
		msgErrf(buffero, "Error binding socket\n");
		return 2;
	}

	sockserv_dozers.status = SOCK_CONNECTED;
	listen(sockserv_dozers.fd, 6);

	msgStdof(buffero, "LISTENING\n");

	// Init ActorData List
	initAdataList();
	initTickerList();

	while (cint_get(finishProgram) == 0) {
		sockcli = accept(sockserv_dozers.fd, (struct sockaddr *) &cliaddr, &clilen);

		// Handle the request
		if (sockcli <= 0) {
			msgErrf(buffero, "Error handling request\n");
			return 3;
		} else {
			Frame *frame;
			frame_new((void **)&frame);
			rwbytes = read(sockcli, frame, sizeof(Frame));
			if (rwbytes <= 0) {
				fdut_msgStdof(buffero, "Error while receiving connection frame, ignoring\n");
				close(sockcli);
				break;
			}

			if (sf_frame_match(frame) == F_DOZ_CONN) {
				char *dozerName = strdup(frame_getSource(frame));

				frame_setSource(frame, GEK_FRAME_SOURCE);
				frame_setType(frame, GEK_FRAME_TYPE_O);
				frame_setData(frame, GEK_FRAME_DATA_O);
				rwbytes = write(sockcli, frame, sizeof(Frame));
				if (rwbytes <= 0) {
					fdut_msgStdof(buffero, "Error while receiving connection frame, ignoring\n");
					close(sockcli);
					break;
				}
				msgStdof(buffero, "New Dozer Connected\n");
				//msgStdof(buffero, "SockCli1: %d\n", sockcli);

				ActorDataS *adataElem;
				// Request successful, search if the dozer is in the list (Actor created or not)
				// Remember that every Actor is identified by the dozer's name
				adataElem = cllist_getFirstOcurrenceOfElement(adataList,
				lambda (int, (void* elem) {
					ActorDataS *adata = (ActorDataS *)elem;
					if (strcmp(dozerName, adatas_getActorName(adata)) == 0) {
						return 0;
					}
					return -1;
				})
				                                             );

				int *epoll;
				struct epoll_event ev;
				int *s;
				int err;

				epoll = adatas_getEpoll(adataElem);

				// If the Actor is not created...
				if (adataElem == NULL) {

					// Create the Actor

					// Create adata: element, pipe
					adatas_new((void **)&adataElem);
					// Set name so the element can be found in the list
					adatas_setActorName(adataElem, dozerName);

					// Add the actor to the list
					cllist_append(adataList, adataElem);

					// Old adataElem (not in the list) is no longer needed, get it from the list instead (see below)
					// The same variable (adataElem) is used so the referencing element must be the same when exiting this condition (actor is in the list, retrieve it)
					adatas_destroy(adataElem);
					adataElem = NULL;

					adataElem = cllist_getFirstOcurrenceOfElement(adataList,
					lambda (int, (void* elem) {
						ActorDataS *adata = (ActorDataS *)elem;
						if (strcmp(dozerName, adatas_getActorName(adata)) == 0) {
							return 0;
						}
						return -1;
					})
					                                             );

					// Initialize inner elements
					pipe(adatas_getPipe(adataElem));

					// Create epoll and fill with events, max. events 15 for safety
					epoll = adatas_getEpoll(adataElem);
					*epoll = epoll_create(ACTOR_MAXEVENTS);

					if (*epoll == -1) {
						msgErrf(buffero, "Error creating epoll, exiting now!\n");
						adatas_destroy(adataElem);
						exitProgram(EXIT_FAILURE);
					}

					// This is the workflow for adding an event
					// Workflow START
					ev.events = EPOLLIN;
					ev.data.fd = adatas_getPipe(adataElem)[0];
					err = epoll_ctl(*epoll, EPOLL_CTL_ADD, adatas_getPipe(adataElem)[0], &ev);
					if (err == -1) {
						msgErrf(buffero, "Error adding epoll event, exiting now!\n");
						exitProgram(EXIT_FAILURE);
					}
					// Workflow END

					// Create the thread
					err = pthread_create(&(adatas_getPthread(adataElem)->id), NULL, thread_ActorS, (void *)adataElem);
					if (err != 0) {
						msgErrf(buffero, "Error creating Actor\n");
						exitProgram(EXIT_FAILURE);
					}
					// From now on, the thread is always active

				}

				// Add the connection to the epoll and to the actor socket

				// Add actor socket
				s = &(adatas_getSocket(adataElem)->fd);
				*s = sockcli;

				// Add event to epoll
				ev.events = EPOLLIN;
				ev.data.fd = *s;
				err = epoll_ctl(*epoll, EPOLL_CTL_ADD, *s, &ev);
				if (err == -1) {
					msgErrf(buffero, "Error adding epoll event, exiting now!\n");
					exitProgram(EXIT_FAILURE);
				}

				free(dozerName);

				msgStdof(buffero, "Dozer %s connected to the system\n", adatas_getActorName(adataElem));
				// Forwarding connection frame to the actor
				frame_setSource(frame, adatas_getActorName(adataElem));
				frame_setType(frame, DOZ_FRAME_TYPE_C);
				frame_setData(frame, DOZ_FRAME_DATA_C);
				write(adatas_getPipe(adataElem)[1], frame, sizeof(Frame));
			} else {
				// Ignore the connection if it is not a dozer
				msgStdof(buffero, "Closing connection. No Dozer detected!\n");
				close(sockcli);
			}
			frame_destroy(frame);
		}
	}

	return 0;

}

int stopServingIfServing()
{
	if (sockserv_dozers.status == SOCK_CONNECTED) {
		char buffero[512];
		msgStdof(buffero, "Closing listening port\n");
		close(sockserv_dozers.fd);
	}

	return 0;
}

int connectToTumblingDiceIfNotConnected(int nRetrys)
{

	char buffero[512];
	struct sockaddr_in *servaddr;
	Frame *frame;
	ssize_t rwbytes;
	int retry;

	if (sockcli_tumblingDice.status == SOCK_DISCONNECTED) {
		msgStdof(buffero, "Starting to connect with server\n");

		servaddr = gek_getTDConfig(gekko);

		sockcli_tumblingDice.fd = socket(AF_INET, SOCK_STREAM, 0);

		if (sockcli_tumblingDice.fd < 0) {
			return -1;
		}

		retry = 0;

		while (retry < nRetrys && sockcli_tumblingDice.status == SOCK_DISCONNECTED) {
			if (cint_get(finishProgram) != 0) {
				return 0;
			}
			if (connect(sockcli_tumblingDice.fd, (struct sockaddr *) servaddr, sizeof(struct sockaddr_in)) < 0) {
				msgErrf(buffero, "Could not connect with server in retry number %d\n", (retry + 1));
				pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
				sleep(1);
				pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
				retry++;
				sockcli_tumblingDice.status = SOCK_DISCONNECTED;
			} else {
				sockcli_tumblingDice.status = SOCK_CONNECTED;
			}
		}

		if (sockcli_tumblingDice.status == SOCK_DISCONNECTED) {
			msgErrf(buffero, "Cannot stablish a connection with Tumbling Dice server\n", retry);
			return -1;
		}

		msgStdof(buffero, "Connection stablished\n");
		msgStdof(buffero, "Veryfying Connection\n");

		// Create frame
		frame = frame_apply(GEK_FRAME_SOURCE, GEK_FRAME_TYPE_C, GEK_FRAME_DATA_C);

		write(sockcli_tumblingDice.fd, frame, sizeof(Frame));
		rwbytes = read(sockcli_tumblingDice.fd, frame, sizeof(Frame));

		if (rwbytes <= 0) {
			msgErrf(buffero, "An error ocurred while verifying connection with Tumbling Dice\n");
			return -1;
		}

		if (sf_frame_match(frame) == F_TD_ERR) {
			msgErrf(buffero, "An error ocurred while verifying connection with Tumbling Dice\n");
			return -1;
		}
		if (!sf_frame_match(frame) == F_TD_CONNOK) {
			msgErrf(buffero, "Unknown Tumbling Dice frame type\n");
			return -1;
		}
		msgStdof(buffero, "Connection verified!\n");

		frame_destroy(frame);

	}

	return 0;
}

int disconnectFromTumblingDiceIfConnected()
{
	if (sockcli_tumblingDice.status == SOCK_CONNECTED) {
		Frame *frame;
		ssize_t rwbytes;
		char buffero[512];

		// Prepare frame
		frame = frame_apply(GEK_FRAME_SOURCE, GEK_FRAME_TYPE_Q, GEK_FRAME_DATA_Q);
		//msgStdof(buffero, "Before closing socket\n");
		// Send frame
		rwbytes = write(sockcli_tumblingDice.fd, frame, sizeof(Frame));
		msgStdof(buffero, "Disconected from Tumbling Dice\n");

		close(sockcli_tumblingDice.fd);
		sockcli_tumblingDice.status = SOCK_DISCONNECTED;
		frame_destroy(frame);

	}
	return 0;
}

void* pth_refreshIbexValues(void* period)
{
	pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);

	int err = 0;
	period = NULL;
	int refreshPeriod;

	// Retrieve the refresh period
	refreshPeriod = gek_getRefreshPeriod(gekko);
	//refreshPeriod = 2;
	//msgStdof(buffero, "IBEX-35 refresh period set to %d for testing purposes\n", refreshPeriod);

	pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
	// Check if the thread has to finish
	while (cint_get(finishProgram) == 0) {

		err = refreshIbexValues();

		sleep(refreshPeriod);

		if (err == -2) {
			err = connectToTumblingDiceIfNotConnected(5);
			if (err == -1) {
				pth_RIV.status = THREAD_NOT_RUNNING;
				exitProgram(EXIT_FAILURE);
			} else if (err == 0) continue;
		} else if (err == -1) {
			pth_RIV.status = THREAD_NOT_RUNNING;
			exitProgram(EXIT_FAILURE);
		} else if (err == 0) continue;
	}
	return NULL;
}


int refreshIbexValues(void)
{
	regex_t regex;
	int reti;
	size_t nmatch;
	regmatch_t *pmatch;

	char buffero[512];
	char auxbuff[256];
	char *bufferi;
	cLinkedList *newIbexValues;
	cLinkedList *oldIbexValues;
	IbVal *ibvalFrame;
	IbVal *ibvalElem;
	Frame *frame;
	int i;
	ssize_t rwbytes = 0;

	void (*freeOnError)(void) = lambda (void, (void) {
		frame_destroy(frame);
		free(pmatch);
		ibval_destroy(ibvalFrame);
	});

	reti = regcomp(&regex, "^(.*)(\\-[0-9]+\\.[0-9][0-9]?|\\+[0-9]+\\.[0-9][0-9]?)$", REG_EXTENDED);
	if (reti) {
		msgErrf(buffero, "Error setting up RFP\n");
		return -1;
	}

	ibval_new((void **)&ibvalFrame);

	nmatch = 3;
	pmatch = (regmatch_t *)malloc(sizeof(regmatch_t) * nmatch);

	// Create a frame
	frame_new((void **)&frame);

	cllist_new((void **)&newIbexValues);
	oldIbexValues = gek_getIbexValues(gekko);
	cllist_copy(newIbexValues, oldIbexValues);

	for (i = 0; i < STANDARD_IBVALS; i++) {
		ibvalElem = NULL;
		// Prepare frame
		frame_setSource(frame, GEK_FRAME_SOURCE);
		frame_setType(frame, GEK_FRAME_TYPE_P);
		frame_setData(frame, GEK_FRAME_DATA_P);

		rwbytes = write(sockcli_tumblingDice.fd, frame, sizeof(Frame));
		// Write may succeed because the program is writting to internal OS buffers
		if (rwbytes <= 0) {
			sockcli_tumblingDice.status = SOCK_DISCONNECTED;
			msgErrf(buffero, "An error ocurred while retrieving Ibex Values, connection to Tumbling Dice has been lost\n");
			freeOnError();
			return -2;
		}
		rwbytes = read(sockcli_tumblingDice.fd, frame, sizeof(Frame));

		if (rwbytes <= 0) {
			sockcli_tumblingDice.status = SOCK_DISCONNECTED;
			msgErrf(buffero, "An error ocurred while retrieving Ibex Values, connection to Tumbling Dice has been lost\n");
			freeOnError();
			return -2;
		}
		if (sf_frame_match(frame) == F_TD_ERR) {
			msgErrf(buffero, "An error ocurred while retrieving Ibex Values, exiting program\n");
			freeOnError();
			return -1;
		}

		bufferi = frame_getData(frame);
		reti = regexec(&regex, bufferi, nmatch, pmatch, 0);

		regex_getRegexString(auxbuff, bufferi, pmatch, 1);
		ibval_setTicker(ibvalFrame, auxbuff);

		regex_getRegexString(auxbuff, bufferi, pmatch, 2);
		ibval_setValue(ibvalFrame, atof(auxbuff));

		ibvalElem = cllist_getFirstOcurrenceOfElement(newIbexValues,
		lambda (int, (void* elem) {
			IbVal *ibval = (IbVal *)elem;
			if (strcmp(ibval_getTicker(ibvalFrame), ibval_getTicker(ibval)) == 0) {
				return 0;
			}
			return -1;
		})
		                                             );

		if (ibvalElem != NULL)  {
			//fdut_msgStdof(buffero, "For Ticker: %s with oldValue %.2f change with %.2f\n", ibval_getTicker(ibvalFrame), ibval_getValue(ibvalElem), ibval_getValue(ibvalFrame));
			ibval_setValue(ibvalElem, ibval_getValue(ibvalElem) + ibval_getValue(ibvalFrame));
			//fdut_msgStdof(buffero, "As result %.2f\n", ibval_getValue(ibvalElem));
		} else {
			//fdut_msgStdof(buffero, "Ticker: %s not found\n", ibval_getTicker(ibvalFrame));
		}
	}

	// Copy the updated list to the gekko
	gek_setIbexValues(gekko, newIbexValues);
	// Destroy old Ibex value list
	cllist_destroy(oldIbexValues);
	// Free some elements
	freeOnError();

	return 0;
}


int readConfigFile(char *filename)
{
	int fd;
	char buffero[512];
	char *bufferi;

	fd = open(filename, O_RDONLY);

	if (fd < 0) {
		fdut_msgStdof(buffero, "Error reading file %s\n", filename);
		return -1;
	}

	bufferi = (char *)malloc(sizeof(char) * 1);
	if (bufferi == NULL) {
		msgErrf(buffero, "Cannot allocate memory\n", bufferi);
	}

	fdut_gets(fd, &bufferi);
	bufferi[strlen(bufferi) - 1] = '\0';
	gek_setRefreshPeriod(gekko, atoi(bufferi));

	fdut_gets(fd, &bufferi);
	bufferi[strlen(bufferi) - 1] = '\0';
	msgStdof(buffero, "Tumbing Dice IP: %s\n", bufferi);
	gek_getTDConfig(gekko)->sin_addr.s_addr = inet_addr(bufferi);

	fdut_gets(fd, &bufferi);
	bufferi[strlen(bufferi) - 1] = '\0';
	msgStdof(buffero, "Tumbling Dice Port: %s\n", bufferi);
	gek_getTDConfig(gekko)->sin_port = htons(atoi(bufferi));

	// Finish socket configuration to TumblingDice
	gek_getTDConfig(gekko)->sin_family = AF_INET;

	close(fd);

	free(bufferi);

	return 0;
}


int readIbexFile(char* filename)
{
	regex_t regex;
	int reti;
	size_t nmatch;
	regmatch_t *pmatch;

	int fd;
	char buffero[512];
	char auxbuff[216];
	char *bufferi;
	size_t rwbytes;
	cLinkedList *gek_ibvals;
	IbVal *ibval;

	fd = open(filename, O_RDONLY);

	if (fd < 0) {
		msgErrf(buffero, "Error reading file %s\n", filename);
		return -1;
	}

	bufferi = (char *)malloc(sizeof(char) * 1);
	if (bufferi == NULL) {
		msgErrf(buffero, "Error allocating memory\n");
		return -1;
	}

	// Read dozer shares

	gek_ibvals = gek_getIbexValues(gekko);

	reti = regcomp(&regex, "^(.+)\t([0-9]+\\.[0-9][0-9]?)?\t([0-9]+)$", REG_EXTENDED);

	if (reti) {
		msgErrf(buffero, "Error reading file %s\n", filename);
		close(fd);
		free(bufferi);
		exitProgram(EXIT_FAILURE);
	}

	nmatch = 4;
	pmatch = (regmatch_t *)malloc(sizeof(regmatch_t) * nmatch);

	ibval_new((void **)&ibval);

	rwbytes = fdut_gets(fd, &bufferi);
	bufferi[strlen(bufferi) - 1] = '\0';

	while (rwbytes != 0) {

		reti = regexec(&regex, bufferi, nmatch, pmatch, 0);

		regex_getRegexString(auxbuff, bufferi, pmatch, 1);
		// Set share ticker
		ibval_setTicker(ibval, auxbuff);

		regex_getRegexString(auxbuff, bufferi, pmatch, 2);
		ibval_setValue(ibval, atof(auxbuff));

		regex_getRegexString(auxbuff, bufferi, pmatch, 3);
		ibval_setNShares(ibval, atof(auxbuff));

		cllist_append(gek_ibvals, ibval);

		rwbytes = fdut_gets(fd, &bufferi);
		bufferi[strlen(bufferi) - 1] = '\0';

	}

	close(fd);
	free(bufferi);
	regfree(&regex);
	free(pmatch);
	free(ibval);

	return 0;
}



void intConf()
{
	// Disable broken pipe interruption, errors handled in-place
	signal (SIGPIPE, SIG_IGN);
	// Redirect SIGINT
	signal(SIGINT, ksighandler);
}

void ksighandler(int sig)
{
	switch (sig) {
	case SIGINT:
		fdut_msgStdo("\n");
		exitProgram(EXIT_FAILURE);
		break;
	default:
		break;
	}
}

void finishThreadsIfCreated()
{
	int s;
	void *res;

	// Join on actor list's threads
	destroyAdataList(adataList);

	// Join on refresh ibex values (RIV) thread
	if (pth_RIV.status == THREAD_RUNNING) {
		//msgStdof(buffero, "RIV join...\n");
		pthread_cancel(pth_RIV.id);
		s = pthread_join(pth_RIV.id, &res);
		pth_RIV.status = THREAD_NOT_RUNNING;
	}
	//msgStdof(buffero, "RIV joined\n");
}

void exitProgramR()
{
	char buffero[512];

	if (cint_get(finishProgram) == 0) {
		msgStdof(buffero, "Sayonara\n");
		msgStdof(buffero, "Wait while the system is shutting down safely...\n");

		cint_set(finishProgram, 1);
		// Delete ticker list
		destroyTickerList(tickerList);
		finishThreadsIfCreated();
		disconnectFromTumblingDiceIfConnected();
		stopServingIfServing();
		gek_destroy(gekko);
	} else {
		msgStdof(buffero, "Forcing to exit\n");
	}
	cint_destroy(finishProgram);
}

void exitProgram(int exitStatus)
{
	exitProgramR();
	exit(exitStatus);
}

ssize_t msgErrf(char *buffer, char *format, ...)
{
	va_list args;
	ssize_t rwbytes;

	rwbytes = fdut_msgf(2, buffer, "[Gekko] - ERROR: ");

	va_start(args, format);
	rwbytes = rwbytes + fdut_vmsgf(1, buffer, format, args);
	va_end(args);

	return rwbytes;
}

ssize_t msgStdof(char *buffer, char *format, ...)
{
	va_list args;
	ssize_t rwbytes;

	rwbytes = fdut_msgf(1, buffer,  "[Gekko] - INFO: ");

	va_start(args, format);
	rwbytes = rwbytes + fdut_vmsgf(1, buffer, format, args);
	va_end(args);

	return rwbytes;
}

void* thread_ActorS(void *arg)
{

	ActorDataS *actor = (ActorDataS *)arg;
	Frame *frame;
	char buffero[1024], auxbuff[512];
	int i;
	ssize_t rwbytes;
	LinkedList *msgToForward;
	llist_new((void**)&msgToForward);
	llist_setElemFuncs(msgToForward,
	                   frame_new,
	                   frame_cpy,
	                   NULL,
	                   frame_destroy);


	struct epoll_event events[ACTOR_MAXEVENTS], ev;
	int nfds;
	void (*disconnect)(void) = lambda (void, (void) {
		frame = frame_apply(adatas_getActorName(actor), DOZ_FRAME_TYPE_Q, DOZ_FRAME_DATA_Q);
		// Send frame
		rwbytes = write(adatas_getPipe(actor)[1], frame, sizeof(Frame));
		frame_destroy(frame);
	});
	void (*disconnectReal)(void) = lambda (void, (void) {
		int err = 0;

		if (adatas_getSocket(actor)->status == SOCK_CONNECTED) {

			msgStdof(buffero, "Disconnecting dozer %s\n", adatas_getActorName(actor));

			ev.data.fd = adatas_getSocket(actor)->fd;
			err = epoll_ctl(*adatas_getEpoll(actor), EPOLL_CTL_DEL, adatas_getSocket(actor)->fd, &ev);
			close(adatas_getSocket(actor)->fd);
			adatas_getSocket(actor)->status = SOCK_DISCONNECTED;

			if (err == -1) {

				msgErrf(buffero, "Error deleting epoll event, exiting now!\n");
				adatas_getPthread(actor)->status = THREAD_NOT_RUNNING;
				exitProgram(EXIT_FAILURE);
			}
		}
	});

	while (1) {
		// Wait for events (frames)
		nfds = epoll_wait(*adatas_getEpoll(actor), events, ACTOR_MAXEVENTS, -1);
		// Iterate through frames received and match them
		for (i = 0; i < nfds; i++) {

			frame_new((void **)&frame);

			rwbytes = read(events[i].data.fd, frame, sizeof(Frame));

			if (rwbytes <= 0 && adatas_getSocket(actor)->fd == events[i].data.fd) {
				msgErrf(buffero, "Error sending frame\n");
				frame_destroy(frame);
				disconnectReal();
				continue;
			}

			//msgStdof(buffero, "received : {source:%s, type:%c, data: %s}\n", frame_getSource(frame), frame_getType(frame), frame_getData(frame));

			if (
			    strcmp(frame_getSource(frame), adatas_getActorName(actor)) != 0
			    && !(sf_frame_match(frame) == F_GEK_SHUTDOWN)
			)
			{
				//msgStdof(buffero, "Actor %s has received an unknown frame\n", adatas_getActorName(actor));
				frame_destroy(frame);
				continue;
			}

			switch (sf_frame_match(frame)) {
			case F_GEK_SHUTDOWN:
			{
				char buffero[512];

				msgStdof(buffero, "Actor %s has received a shutdown message\n", adatas_getActorName(actor));

				disconnectReal();
				return NULL;
			}
			break;
			case F_DOZ_CONN:
			{
				char buffero[512];

				adatas_getSocket(actor)->status = SOCK_CONNECTED;

				fdut_msgStdof(buffero, "From actor %s, dozer connected\n", adatas_getActorName(actor));

				llist_foreach(msgToForward, lambda (int, (void* elem)
				{
					Frame *_frame = (Frame *)elem;
					rwbytes = write(adatas_getSocket(actor)->fd, _frame, sizeof(Frame));
					if (rwbytes <= 0) {
						msgErrf(buffero, "Error sending frame\n");
						disconnect();
					}

					return 0;
				})
				             );


				llist_destroy(msgToForward);
				llist_new((void**)&msgToForward);
				llist_setElemFuncs(msgToForward,
				                   frame_new,
				                   frame_cpy,
				                   NULL,
				                   frame_destroy);

			}
			break;
			case F_DOZ_DISCONN:
			{
				/*
				Dozer disconnected
				- Remove socket from epoll
				- Close socket
				*/
				disconnectReal();
			}
			break;
			case F_DOZ_SHOW_IBEX:
			{

				cLinkedList *ibexList = NULL;
				char data[100];

				cllist_new((void **)&ibexList);
				cllist_copy(ibexList, gek_getIbexValues(gekko));

				cllist_zipWithIndex(ibexList, tickerList,
				lambda (int, (void *ibval, void *tickerSell, int index) {
					unsigned int tickerSellSum;
					// Really?...
					index = index;
					IbVal *_ibval = (IbVal *)ibval;
					TickerSell *_tickerSell = (TickerSell *)tickerSell;

					tickerSellSum = 0;
					if (_tickerSell == NULL) msgStdof(buffero, "Ei, soy NULL!\n");

					cllist_foreach(ts_getDozerList(_tickerSell),
					lambda (int, (void *share) {
						Share *_share = (Share *)share;
						tickerSellSum += share_getValue(_share);
						return 0;
					})
					              );


					sprintf(data, "%s\n%.2f\n%.0lf",
					        ibval_getTicker(_ibval),
					        ibval_getValue(_ibval),
					        ibval_getNShares(_ibval) + tickerSellSum
					       );
					frame_setSource(frame, GEK_FRAME_SOURCE);
					frame_setType(frame, GEK_FRAME_TYPE_X);
					frame_setData(frame, data);
					rwbytes = write(adatas_getSocket(actor)->fd, frame, sizeof(Frame));
					if (rwbytes <= 0) {
						msgErrf(buffero, "Error receiving frame, ignoring!\n");
						disconnect();
						return -1;
					}
					return 0;

				})
				                   );


				cllist_destroy(ibexList);

			}
			break;
			case F_DOZ_BUY:
			{
				regex_t regex;
				int reti;
				size_t nmatch;
				regmatch_t *pmatch;

				struct {
					char ticker[20];
					int num;
					float dozerMoney;
				} request;

				BuyResponse response;

				// Initialize response
				response.status = -1;
				response.total = 0;
				response.price = -2;
				response.num = -1;
				strcpy(response.ticker, "unknown");

				IbVal *ibvalElem = NULL;

				msgStdof(buffero, "Dozer %s wants to buy something...\n", adatas_getActorName(actor));

				reti = regcomp(&regex, "^(.+)\n(.+)\n(.+)$", REG_EXTENDED);

				if (reti) {
					frame_destroy(frame);
					msgErrf(buffero, "Error parsing frame, exiting now!");
					pthread_exit(0);
					exitProgram(EXIT_FAILURE);
				}

				nmatch = 4;
				pmatch = (regmatch_t *)malloc(sizeof(regmatch_t) * nmatch);

				reti = regexec(&regex, frame_getData(frame), nmatch, pmatch, 0);
				if (reti == REG_NOMATCH) {
					msgErrf(buffero, "Malformed buy request, ignoring\n");
					regfree(&regex);
					free(pmatch);
					break;
				}

				//msgStdof(buffero, "Buy: Regex Match\n");

				regex_getRegexString(auxbuff, frame_getData(frame), pmatch, 1);
				if (strlen(auxbuff) >= 20) {
					msgStdof(buffero, "Malformed buy request, ignoring\n");
					break;
				}
				strcpy(request.ticker, auxbuff);

				regex_getRegexString(auxbuff, frame_getData(frame), pmatch, 2);
				request.num = atoi(auxbuff);

				regex_getRegexString(auxbuff, frame_getData(frame), pmatch, 3);
				request.dozerMoney = atof(auxbuff);

				//msgStdof(buffero, "{ticker: %s, num: %d, dozerMoney: %f}\n", request.ticker, request.num, request.dozerMoney);

				TickerSell *ts;
				cLinkedList *dozerlist;
				LinkedList *dozermsg;
				int amount;

				amount = request.num;
				response.num = request.num;

				llist_new((void**)&dozermsg);
				llist_setElemFuncs(dozermsg,
				                   frame_new,
				                   frame_cpy,
				                   NULL,
				                   frame_destroy);

				cllist_sync(gek_getIbexValues(gekko),
				            lambda (int, (LinkedList * llist)
				{
					// Checking for valid ticker name
					ibvalElem = llist_getFirstOcurrenceOfElement(llist,
					            lambda (int, (void *elem)
					{
						IbVal *ibval = (IbVal *)elem;
						if (strcmp(ibval_getTicker(ibval), request.ticker) == 0) {
							return 0;
						}
						return -1;
					})
					                                            );
					if (ibvalElem == NULL) {
						//msgStdof(buffero, "The ticker does not exist, ticker: %s\n", request.ticker);
						// The ticker does not exist
						strcpy(response.ticker, "unknown");
						response.status = -1;
					} else {
						strcpy(response.ticker, ibval_getTicker(ibvalElem));
						response.price = ibval_getValue(ibvalElem);
						response.status = 1;
					}
					return 0;
				})
				           );

				// The ticker exists
				if (response.status != -1) {
					// Calculate money
					response.total = response.price * request.num;
					float moneyToSpend = request.dozerMoney - response.total;
					if (moneyToSpend <= -1) {
/*						msgStdof(buffero,
						         "The dozer does not have enough money, ibval price: %f, num: %d, total: %f, money to spend: %f\n",
						         response.total,
						         moneyToSpend
						        );*/
						// The dozer don't have enough money
						response.total = -1;
						response.status = -1;

					} else {
						response.status = 1;
					}

				}

				if ( response.status == 1) {
					// Try first to buy dozer offers
					ts = cllist_getFirstOcurrenceOfElement(tickerList,
					                                       lambda (int, (void* elem)
					{
						TickerSell *_ts = (TickerSell *)elem;
						if (strcmp(request.ticker, ts_getTicker(_ts)) == 0) {
							return 0;
						}
						return -1;
					})
					                                      );

					dozerlist = ts_getDozerList(ts);

					cllist_foreach(dozerlist, lambda (int, (void* elem)
					{
						int sharevalue;
						Frame * msg;
						Share *share = (Share *)elem;
						if (amount > 0) {
							// The dozer has shares to sell for this ticker
							if (share_getValue(share) != 0) {
								if (amount >= share_getValue(share))
								{
									sharevalue = share_getValue(share);
									amount = amount - share_getValue(share);
									share_setValue(share, 0);
								} else {
									share_setValue(share, share_getValue(share) - amount);
									sharevalue = amount;
									amount = 0;
								}
								sprintf(buffero, "%d\n%s\n%f", sharevalue, ts_getTicker(ts), (sharevalue * response.price));
								msg = frame_apply(share_getTicker(share), GEK_FRAME_TYPE_M, buffero);
								llist_append(dozermsg, msg);
							}
							return 0;
						}
						return 1;

					})
					              );

					if (amount != 0) {

						cllist_sync(gek_getIbexValues(gekko),
						            lambda (int, (LinkedList * llist)
						{
							// Seriously? Warning? -.-
							llist = llist;
							if (ibvalElem != NULL) {
								double remainingShares = ibval_getNShares(ibvalElem) - amount;
								if (remainingShares < 0) {
									//msgStdof(buffero, "Not enough shares, remaining Shares: %lu\n", remainingShares);
									response.num = -1;
									response.status = -1;
								}

								if (response.status == -1) return response.status;

								// Perform operation if parameters are correct
								ibval_setNShares(ibvalElem, ibval_getNShares(ibvalElem) - amount);
								response.status = 1;
								return response.status;
							}
							return response.status;

						})
						           );
					}
				}

				llist_foreach(dozermsg, lambda (int, (void* elem)
				{
					Frame *msg = (Frame *)elem;
					char *dozerName = frame_getSource(msg);
					ActorDataS *actor;
					actor = cllist_getFirstOcurrenceOfElement(adataList, lambda (int, (void* elem)
					{
						ActorDataS *_actor = (ActorDataS *)elem;
						if (strcmp(dozerName, adatas_getActorName(_actor)) == 0) {
							return 0;
						}
						return -1;
					}));

					rwbytes = write(adatas_getPipe(actor)[1], msg, sizeof(Frame));
					if (rwbytes <= 0) {
						msgErrf(buffero, "An error ocurred while finishing program, exiting now\n");
						exit(EXIT_FAILURE);
					}
					return 0;
				})
				             );

				llist_destroy(dozermsg);

				frame_setSource(frame, GEK_FRAME_SOURCE);
				frame_setType(frame, GEK_FRAME_TYPE_B);

				//msgStdof(buffero, "Result: %d\n", response.status);
				sprintf(buffero, "%d\n%s\n%d\n%.2f\n%.2f", response.status, response.ticker, response.num, response.price, response.total);
				frame_setData(frame, buffero);
				rwbytes = write(adatas_getSocket(actor)->fd, frame, sizeof(Frame));
				if (rwbytes <= 0) {
					disconnect();
					break;
				}

			}
			break;
			case F_DOZ_SELL:
			{
				regex_t regex;
				int reti;
				size_t nmatch;
				regmatch_t *pmatch;

				struct {
					char ticker[20];
					int num;
				} request;

				msgStdof(buffero, "Dozer %s wants to sell something...\n", adatas_getActorName(actor));

				//** Reading the frame **//

				reti = regcomp(&regex, "^(.+)\n(.+)$", REG_EXTENDED);

				if (reti) {
					frame_destroy(frame);
					msgErrf(buffero, "Error parsing frame, exiting now!");
					pthread_exit(0);
					exitProgram(EXIT_FAILURE);
				}

				nmatch = 3;
				pmatch = (regmatch_t *)malloc(sizeof(regmatch_t) * nmatch);

				reti = regexec(&regex, frame_getData(frame), nmatch, pmatch, 0);
				if (reti == REG_NOMATCH) {
					msgErrf(buffero, "Malformed buy request, ignoring\n");
					regfree(&regex);
					free(pmatch);
					break;
				}

				//** Filling the list **//

				regex_getRegexString(auxbuff, frame_getData(frame), pmatch, 1);
				strcpy(request.ticker, auxbuff);
				regex_getRegexString(auxbuff, frame_getData(frame), pmatch, 2);
				request.num = atoi(auxbuff);

				//msgStdof(buffero, "{ticker: %s, num: %d}\n", request.ticker, request.num);

				TickerSell *ts;

				// Loop the TickerList to find the Ticker name and introduce the values of the new sell
				ts = cllist_getFirstOcurrenceOfElement(tickerList,
				                                       lambda (int, (void* elem)
				{
					TickerSell *_ts = (TickerSell *)elem;
					if (strcmp(request.ticker, ts_getTicker(_ts)) == 0) {
						return 0;
					}
					return -1;
				})
				                                      );

				//msgStdof(buffero, "\nfound ticker:%s\n", ts_getTicker(ts));

				// en el elemento ts anoto el nombre del dozer que vende y la cantidad --> utilizar el share

				cLinkedList *dozerlist;
				Share *dozoffer;
				int amount;

				dozerlist = ts_getDozerList(ts);

				dozoffer = cllist_getFirstOcurrenceOfElement(dozerlist,
				           lambda (int, (void* elem)
				{
					Share * sh = (Share *)elem;
					if (strcmp(frame_getSource(frame), share_getTicker(sh)) == 0) {
						return 0;
					}
					return -1;
				})
				                                            );

				if ( dozoffer == NULL ) {

					share_new((void**)&dozoffer);
					share_setTicker(dozoffer, frame_getSource(frame));
					share_setValue(dozoffer, request.num);
					cllist_append(dozerlist, dozoffer);
					share_destroy(dozoffer);
				}
				else {

					cllist_sync(dozerlist,
					            lambda(int, (LinkedList * llist)
					{
						llist = llist;
						amount = share_getValue(dozoffer);
						amount = amount + request.num;
						return share_setValue(dozoffer, amount);
					})
					           );

				}

				frame_setSource(frame, GEK_FRAME_SOURCE);
				frame_setType(frame, DOZ_FRAME_TYPE_S);
				frame_setData(frame, "-");

				rwbytes = write(adatas_getSocket(actor)->fd, frame, sizeof(Frame));
				if (rwbytes <= 0) {
					disconnect();
					break;
				}

			}
			break;
			case F_GEK_SELL_M:
			{
				//msgStdof(buffero, "Frame M recieved by %s\n", adatas_getActorName(actor));
				frame_setSource(frame, GEK_FRAME_SOURCE);

				if (adatas_getSocket(actor)->status == SOCK_CONNECTED) {
					rwbytes = write(adatas_getSocket(actor)->fd, frame, sizeof(Frame));
				} else if (adatas_getSocket(actor)->status == SOCK_DISCONNECTED) {
					llist_append(msgToForward, frame);
				}
			}
			break;
			default:
				break;

			}

			frame_destroy(frame);
		}
	}
}


