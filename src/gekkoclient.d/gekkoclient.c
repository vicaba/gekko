/**
 * @author: Victor Caballero (vicaba)
 */
#include <regex.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>

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
#include "../util/socket/socket.h"
#include "../util/cLinkedList/cLinkedList.h"
#include "../util/regex/regex.h"
#include "../util/FP/FP.h"

#include "../IbexValue/IbexValue.h"

#include "./dozer_shell.h"
#include "../Dozer/Dozer.h"
#include "../cShare/cShare.h"
#include "../Share/Share.h"
#include "../Frame/Frame.h"
#include "./client_frames.h"
#include "./ActorDataC/ActorDataC.h"

#define ACTOR_MAXEVENTS 15

static Dozer *dozer;

static stat_sock_t sockcli;

static ActorDataC *adata = NULL;

int readConfigFile(char *filename);

int readStockFile(char *filename);

int connectToGekko(int nRetrys);

int disconnectFromGekkoIfConnected();

void initAdata();

void destroyAdata();

int doShowStock();

int doShowMeTheMoney();

int doShowIbex();

int doBuyTicker(char *ticker, int num);

int doSellTicker(char *ticker, int num);

void intConf();

void ksighandler(int sig);

int saveState();

void exitProgramR();

void exitProgram(int exitStatus);

ssize_t msgErrf(char *buffer, char *format, ...);

ssize_t msgStdof(char *buffer, char *format, ...);

ssize_t prompt();

void* thread_ActorC(void *arg);

int parseSoldFrame(char *data, int *nShares, float *revenue, char *ticker);

int parseIbexFrame(char *data, LinkedList *ibex35List);

int parseBuyFrame(char *data, BuyResponse *response);

void handleGekkoDisconnection();

int main(void)
{
	char buffero[512], *bufferi;
	int exitMenu;
	int err;
	char **cmdArgv;
	int cmdArgc;
	int i;

	sockcli.status = SOCK_DISCONNECTED;
	doz_new((void **)&dozer);
	exitMenu = 0;

	intConf();

	err = 0;
	err += readConfigFile("config.dat");
	err += readStockFile("stock.dat");
	if (err) {
		fdut_msg(2, "Could not read configuration files\n");
		exitProgram(EXIT_FAILURE);
	}

	err = connectToGekko(5);

	if (err == -1) {
		fdut_msg(2, "Could not connect to Gekko, exiting now!\n");
		exitProgram(EXIT_FAILURE);
	}

	initAdata();

	bufferi = (char *)malloc(sizeof(char) * 1);

	msgStdof(buffero, "Connected to Gekko, starting shell\n");

	prompt();

	do {
		fdut_getsStdi(&bufferi);
		bufferi[strlen(bufferi) - 1] = '\0';

		fdut_msgStdo("\n");
		switch (dshell_parseShellCommand(bufferi, &cmdArgc, &cmdArgv)) {
		case DSHELL_SHOW_STOCK:
			doShowStock();
			break;
		case DSHELL_SHOW_ME_THE_MONEY:
			doShowMeTheMoney();
			break;
		case DSHELL_SHOW_IBEX:
			doShowIbex();
			break;
		case DSHELL_BUY_TICKER:
			doBuyTicker(cmdArgv[0], atoi(cmdArgv[1]));
			for (i = 0; i < cmdArgc; i++) {
				free(cmdArgv[i]);
			}
			break;
		case DSHELL_SELL_TICKER:
			doSellTicker(cmdArgv[0], atoi(cmdArgv[1]));
			for (i = 0; i < cmdArgc; i++) {
				free(cmdArgv[i]);
			}
			break;
		case DSHELL_EXIT:
			exitMenu = 1;
			break;
		case DSHELL_CANOT_START_SHELL:
			fdut_msgStdof(buffero, "Could not start dozer shell\n");
			exitMenu = 1;
			break;
		default:
			fdut_msgStdof(buffero, "Invalid command, try again\n");
			prompt();
			break;
		}
	} while (!exitMenu);

	exitProgramR();
	free(bufferi);

	return 1;
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

	fdut_gets(fd, &bufferi);
	bufferi[strlen(bufferi) - 1] = '\0';
	doz_getServerConfig(dozer)->sin_addr.s_addr = inet_addr(bufferi);

	fdut_gets(fd, &bufferi);
	bufferi[strlen(bufferi) - 1] = '\0';
	doz_getServerConfig(dozer)->sin_port = htons(atoi(bufferi));

	doz_getServerConfig(dozer)->sin_family = AF_INET;

	close(fd);

	free(bufferi);

	return 0;
}

int readStockFile(char* filename)
{

	regex_t regex;
	int reti;
	size_t nmatch;
	regmatch_t *pmatch;

	int fd;
	char buffero[512];
	char auxbuff[512];
	char *bufferi;
	size_t rwbytes;
	cLinkedList *doz_shares;
	cShare *share;

	fd = open(filename, O_RDONLY);

	if (fd < 0) {
		fdut_msgStdof(buffero, "Error reading file %s\n", filename);
		return -1;
	}

	bufferi = (char *)malloc(sizeof(char) * 1);

	// Read dozer name
	fdut_gets(fd, &bufferi);
	bufferi[strlen(bufferi) - 1] = '\0';
	doz_setName(dozer, bufferi);


	// Read dozer money
	fdut_gets(fd, &bufferi);
	bufferi[strlen(bufferi) - 1] = '\0';
	cfloat_set(doz_getMoney(dozer), atof(bufferi));

	// Read dozer shares
	doz_shares = doz_getShares(dozer);

	reti = regcomp(&regex, "^(.*)-([0-9]+)$", REG_EXTENDED);

	if (reti) {
		fdut_msgStdof(buffero, "Cannot start dozer shell\n");
		close(fd);
		free(bufferi);
		exitProgram(EXIT_FAILURE);
	}

	nmatch = 3;
	pmatch = (regmatch_t *)malloc(sizeof(regmatch_t) * nmatch);

	cshare_new((void **)&share);

	rwbytes = fdut_gets(fd, &bufferi);
	bufferi[strlen(bufferi) - 1] = '\0';

	while (rwbytes != 0) {

		reti = regexec(&regex, bufferi, nmatch, pmatch, 0);

		strncpy(auxbuff, bufferi + pmatch[1].rm_so, pmatch[1].rm_eo - pmatch[1].rm_so);
		auxbuff[(pmatch[1].rm_eo - pmatch[1].rm_so)] = '\0';

		// Set share name
		cshare_setTicker(share, auxbuff);

		strncpy(auxbuff, bufferi + pmatch[2].rm_so, pmatch[2].rm_eo - pmatch[2].rm_so);
		auxbuff[(pmatch[2].rm_eo - pmatch[2].rm_so)] = '\0';
		cshare_setValue(share, atoi(auxbuff));

		cllist_append(doz_shares, share);

		rwbytes = fdut_gets(fd, &bufferi);
		bufferi[strlen(bufferi) - 1] = '\0';
	}


	close(fd);
	free(bufferi);
	regfree(&regex);
	free(pmatch);
	free(share);

	return 0;
}

void initAdata()
{
	int *epoll;
	struct epoll_event ev;
	int *s;
	char buffero[512];
	int err;

	adatac_new((void **)&adata);

	adatac_setActorName(adata, doz_getName(dozer));
	pipe(adatac_getPipe(adata));

	// Create epoll and fill with events, max. events 15 for safety
	epoll = adatac_getEpoll(adata);
	*epoll = epoll_create(ACTOR_MAXEVENTS);

	if (*epoll == -1) {
		msgErrf(buffero, "Error creating epoll, exiting now!\n");
		exitProgram(EXIT_FAILURE);
	}

	// This is the workflow for adding an event
	// Workflow START
	ev.events = EPOLLIN;
	ev.data.fd = adatac_getPipe(adata)[0];
	err = epoll_ctl(*epoll, EPOLL_CTL_ADD, adatac_getPipe(adata)[0], &ev);
	if (err == -1) {
		msgErrf(buffero, "Error adding epoll event, exiting now!\n");
		exitProgram(EXIT_FAILURE);
	}
	// Workflow END

	s = adatac_getSocket(adata);
	*s = sockcli.fd;
	// Workflow START
	ev.events = EPOLLIN;
	ev.data.fd = *adatac_getSocket(adata);
	err = epoll_ctl(*epoll, EPOLL_CTL_ADD, *adatac_getSocket(adata), &ev);
	if (err == -1) {
		msgErrf(buffero, "Error adding epoll event, exiting now!\n");
		exitProgram(EXIT_FAILURE);
	}
	// Workflow END

	err = pthread_create(&(adatac_getPthread(adata)->id), NULL, thread_ActorC, (void *)adata);
	adatac_getPthread(adata)->status = THREAD_RUNNING;

	if (err != 0) {
		adatac_getPthread(adata)->status = THREAD_NOT_RUNNING;
		msgErrf(buffero, "Error creating Actor\n");
		exitProgram(EXIT_FAILURE);
	}

}

void destroyAdata()
{
	if (adata != NULL) adatac_destroy(adata);
}

int doShowStock()
{
	char buffero[512];
	int i, size;
	cShare *share;
	cLinkedList *doz_shares = doz_getShares(dozer);

	doz_shares = doz_getShares(dozer);
	size = cllist_size(doz_shares);
	for (i = 0; i < size; i++) {
		share = (cShare *)cllist_getElementByIndex(doz_shares, i);
		fdut_msgStdof(buffero, "%s-%d\n", cshare_getTicker(share), cshare_getValue(share));
	}

	prompt();

	return 1;
}

int doShowMeTheMoney()
{
	char buffero[512];
	fdut_msgStdof(buffero, "%.2f€\n", cfloat_get(doz_getMoney(dozer)));
	prompt();
	return 1;
}

int doShowIbex()
{
	Frame *frame;
	frame = frame_apply(doz_getName(dozer), DOZ_FRAME_TYPE_X, DOZ_FRAME_DATA_X);
	write(adatac_getPipe(adata)[1], frame, sizeof(Frame));
	return 0;
}

int doBuyTicker(char *ticker, int num)
{
	char buffero[512];
	Frame *frame;
	ssize_t rwbytes;

	sprintf(buffero, "%s\n%d\n%.2f\n", ticker, num, cfloat_get(doz_getMoney(dozer)));

	frame = frame_apply(doz_getName(dozer),
	                    DOZ_FRAME_TYPE_B,
	                    buffero);

	rwbytes = write(adatac_getPipe(adata)[1], frame, sizeof(Frame));

	if (rwbytes <= 0) {

		msgErrf(buffero, "An error has ocurred while trying to buy shares, exiting now!\n");
		frame_destroy(frame);
		exitProgram(EXIT_FAILURE);

	}
	return 0;
}

int doSellTicker(char *ticker, int num)
{
	char buffero[512];
	Frame *frame;
	ssize_t rwbytes;
	// Check if ticker exists
	cLinkedList *shares = doz_getShares(dozer);
	cShare *shareElem = NULL;

	shareElem = cllist_getFirstOcurrenceOfElement(shares,
	lambda (int, (void *elem) {
		cShare *share = (cShare *)elem;
		if (strcmp(ticker, cshare_getTicker(share)) == 0) {
			return 0;
		}
		return -1;
	})
	                                             );

	if (shareElem == NULL) {

		msgStdof(buffero, "You dont have this share, ignoring request\n");
		prompt();

		return 0;

	} else {
		// Check if dozer have enogh shares to sell
		if (num > cshare_getValue(shareElem)) {
			msgStdof(buffero, "Tickername: %s, NShares: %d, num: %d\n", cshare_getTicker(shareElem), cshare_getValue(shareElem), num);

			msgStdof(buffero, "... Traitor ...\n");
			prompt();

			return 0;

		}
	}

	sprintf(buffero, "%s\n%d", ticker, num);

	frame = frame_apply(doz_getName(dozer),
	                    DOZ_FRAME_TYPE_S,
	                    buffero);

	rwbytes = write(adatac_getPipe(adata)[1], frame, sizeof(Frame));

	//fdut_msgStdof(buffero, "{source: %s, type: %c, data: %s}\n", frame_getSource(frame), frame_getType(frame), frame_getData(frame));

	if (rwbytes <= 0) {

		msgErrf(buffero, "An error has ocurred while trying to buy shares, exiting now!\n");
		frame_destroy(frame);
		exitProgram(EXIT_FAILURE);

	}

	fdut_msgStdof(buffero, "Selling %d shares of ticker %s\n", num, ticker);
	return 0;
}

int disconnectFromGekkoIfConnected()
{
	if (sockcli.status == SOCK_CONNECTED) {
		Frame *frame;
		ssize_t rwbytes;

		// Prepare frame
		frame = frame_apply(doz_getName(dozer), DOZ_FRAME_TYPE_Q, DOZ_FRAME_DATA_Q);
		// Send frame
		rwbytes = write(adatac_getPipe(adata)[1], frame, sizeof(Frame));
		frame_destroy(frame);
	}
	return 0;
}

int connectToGekko(int nRetrys) {
	char buffero[512];
	struct sockaddr_in *servaddr;
	Frame *frame;
	ssize_t rwbytes;
	int retry;

	msgStdof(buffero, "Starting to connect with server Gekko\n");

	servaddr = doz_getServerConfig(dozer);

	sockcli.fd = socket(AF_INET, SOCK_STREAM, 0);

	if (sockcli.fd < 0) {
		return -1;
	}

	retry = 0;

	while (retry < nRetrys && sockcli.status == SOCK_DISCONNECTED) {
		if (connect(sockcli.fd, (struct sockaddr *) servaddr, sizeof(struct sockaddr_in)) < 0) {
			msgErrf(buffero, "Could not connect with server in retry number %d\n", (retry + 1));
			sleep(1);
			retry++;
			sockcli.status = SOCK_DISCONNECTED;
		} else {
			sockcli.status = SOCK_CONNECTED;
		}
	}

	if (sockcli.status == SOCK_DISCONNECTED) {
		msgErrf(buffero, "Cannot stablish a connection with Gekko server\n", retry);
		return -1;
	}

	msgStdof(buffero, "Connection stablished\n");
	msgStdof(buffero, "Veryfying Connection\n");

	// Prepare frame
	frame = frame_apply(doz_getName(dozer), DOZ_FRAME_TYPE_C, DOZ_FRAME_DATA_C);

	write(sockcli.fd, frame, sizeof(Frame));
	rwbytes = read(sockcli.fd, frame, sizeof(Frame));

	if (rwbytes <= 0) {
		msgErrf(buffero, "An error ocurred while verifying connection with Gekko\n");
		return -1;
	}
	if (cf_frame_match(frame) == F_GEK_ERR) {
		msgErrf(buffero, "An error ocurred while verifying connection with Gekko\n");
		return -1;
	}
	if (!cf_frame_match(frame) == F_GEK_CONNOK) {
		msgErrf(buffero, "Unknown Gekko frame type\n");
		return -1;
	}
	msgStdof(buffero, "Connection verified!\n");

	frame_destroy(frame);

	return 0;
}


void intConf()
{
	// Disable broken pipe interruption, errors handled in-place
	signal (SIGPIPE, SIG_IGN);
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

int saveState()
{
	int fd;
	char buffero[512];
	fd = open("stock.dat", O_WRONLY | O_CREAT | O_TRUNC, 0777);
	if (fd < 0) {
		return -1;
	}
	fdut_msgf(fd, buffero, "%s\n", doz_getName(dozer));
	fdut_msgf(fd, buffero, "%.2f\n", cfloat_get(doz_getMoney(dozer)));
	cllist_foreach(doz_getShares(dozer),
	               lambda (int, (void *elem)
	{
		cShare *share = (cShare *)elem;
		fdut_msgf(fd, buffero, "%s-%d\n", cshare_getTicker(share), cshare_getValue(share));
		return 0;
	})
	              );
	close(fd);
	return 0;
}

void exitProgramR()
{
	char buffero[512];
	disconnectFromGekkoIfConnected();
	if (adata != NULL) {
		msgStdof(buffero, "Saving state...\n");
		if (saveState() == -1) {
			msgErrf(buffero, "An error ocurred while saving state, sorry :(\n");
		}
	}

	destroyAdata();
	fdut_msgStdo("Sayonara\n");
	doz_destroy(dozer);
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

	rwbytes = fdut_msgf(2, buffer, "[Dozer-%s] - ERROR: ", doz_getName(dozer));

	va_start(args, format);
	rwbytes = rwbytes + fdut_vmsgf(1, buffer, format, args);
	va_end(args);

	return rwbytes;
}

ssize_t msgStdof(char *buffer, char *format, ...)
{
	va_list args;
	ssize_t rwbytes;

	rwbytes = fdut_msgf(1, buffer,  "[Dozer-%s] - INFO: ", doz_getName(dozer));

	va_start(args, format);
	rwbytes = rwbytes + fdut_vmsgf(1, buffer, format, args);
	va_end(args);

	return rwbytes;
}

ssize_t prompt()
{
	char buffero[512];
	return fdut_msgStdof(buffero, "%s > ", doz_getName(dozer));
}

void* thread_ActorC(void *arg)
{
	ActorDataC *actor = (ActorDataC *)arg;
	Frame *frame;
	char buffero[512];
	int i, err;
	ssize_t rwbytes;

	struct epoll_event events[ACTOR_MAXEVENTS], ev;
	int nfds;

	frame_new((void **)&frame);

	while (1) {


		nfds = epoll_wait(*adatac_getEpoll(actor), events, ACTOR_MAXEVENTS, -1);
		// Iterate through frames received and match them
		for (i = 0; i < nfds; i++) {

			rwbytes = read(events[i].data.fd, frame, sizeof(Frame));

			//msgStdof(buffero, "received : {source:%s, type:%c, data: %s}\n", frame_getSource(frame), frame_getType(frame), frame_getData(frame));

			if (rwbytes <= 0) {

				msgErrf(buffero, "Disconnected from Gekko, exiting now\n");
				frame_destroy(frame);
				adatac_getPthread(actor)->status = THREAD_NOT_RUNNING;
				handleGekkoDisconnection();
				exitProgram(EXIT_FAILURE);
				pthread_exit(0);

			}

			switch (cf_frame_match(frame)) {
			case F_DOZ_DECONN:
			{

				rwbytes = write(*adatac_getSocket(actor), frame, sizeof(Frame));

				if (rwbytes > 0) {

					close(sockcli.fd);
					sockcli.status = SOCK_DISCONNECTED;

				}

				msgStdof(buffero, "Disconnected from Gekko, successful\n");
				err = epoll_ctl(*adatac_getEpoll(actor), EPOLL_CTL_DEL, *adatac_getSocket(actor), &ev);
				pthread_exit(0);

			}
			break;
			case F_DOZ_SHOW_IBEX:
			{

				rwbytes = write(*adatac_getSocket(actor), frame, sizeof(Frame));

				if (rwbytes <= 0) {

					msgErrf(buffero, "Disconnected from Gekko, exiting now\n");
					frame_destroy(frame);
					adatac_getPthread(actor)->status = THREAD_NOT_RUNNING;
					handleGekkoDisconnection();
					exitProgram(EXIT_FAILURE);
					pthread_exit(0);

				}

				//create and fill the list
				LinkedList *ibex35List;
				llist_new((void **)&ibex35List);
				llist_setElemFuncs(ibex35List, ibval_new, ibval_cpy, NULL, ibval_destroy);
				int i;
				for (i = 0; i < 35; i++) {

					rwbytes = read(*adatac_getSocket(actor), frame, sizeof(Frame));

					if (rwbytes <= 0) {

						msgErrf(buffero, "Disconnected from Gekko, exiting now\n");
						cllist_destroy(ibex35List);
						frame_destroy(frame);
						adatac_getPthread(actor)->status = THREAD_NOT_RUNNING;
						handleGekkoDisconnection();

						exitProgram(EXIT_FAILURE);
						pthread_exit(0);

					}

					if (cf_frame_match(frame) == F_GEK_IBEXOK) {

						int err = parseIbexFrame(frame_getData(frame), ibex35List);

						if (err == -1) {

							msgErrf(buffero, "Error parsing frame (show ibex)\n");

						}
					}
				}
				//Show the ibex35List
				IbVal *ibval = NULL;

				int n = llist_size(ibex35List);

				for (i = 0; i < n; i++) {

					ibval = llist_getElementByIndex(ibex35List, i);
					fdut_msgStdof(buffero, "%-10s %-10.2f %-10.0lf \n",
					              ibval_getTicker(ibval),
					              ibval_getValue(ibval),
					              ibval_getNShares(ibval)
					             );

				}

				// Something weird is happening. When I get a rwbytes error, execution reaches
				// this line. Therefore, I have to check if an error ocurred before doing these things
				// this line
				if (rwbytes > 0) {
					llist_destroy(ibex35List);
					prompt();

				}

			}
			break;
			case F_DOZ_BUY_D:
			{

				BuyResponse response;

				rwbytes = write(*adatac_getSocket(actor), frame, sizeof(Frame));

				if (rwbytes <= 0) {

					msgErrf(buffero, "Disconnected from Gekko, exiting now\n");
					frame_destroy(frame);
					adatac_getPthread(actor)->status = THREAD_NOT_RUNNING;
					handleGekkoDisconnection();
					exitProgram(EXIT_FAILURE);
					pthread_exit(0);

				}

				rwbytes = read(*adatac_getSocket(actor), frame, sizeof(Frame));

				if (rwbytes <= 0) {

					msgErrf(buffero, "Disconnected from Gekko, exiting now\n");
					frame_destroy(frame);
					adatac_getPthread(actor)->status = THREAD_NOT_RUNNING;
					handleGekkoDisconnection();
					exitProgram(EXIT_FAILURE);
					pthread_exit(0);

				}

				if (cf_frame_match(frame) == F_DOZ_BUY_G && rwbytes > 0) {

					int result = parseBuyFrame(frame_getData(frame), &response);


					if (result == 1) {
						msgStdof(buffero, "Summary\n");
						if (response.status == 1) {
							fdut_msgStdof(buffero, "Share price: %.2f\n", response.price);

							cfloat_set(doz_getMoney(dozer), cfloat_get(doz_getMoney(dozer)) - response.total);

							// Add bought shares to dozer
							cLinkedList *shares = doz_getShares(dozer);
							cShare *shareElem;
							shareElem = cllist_getFirstOcurrenceOfElement(shares,
							lambda (int, (void *elem) {
								cShare *_share = (cShare *)elem;
								if (strcmp(response.ticker, cshare_getTicker(_share)) == 0) {
									return 0;
								}
								return 1;
							})
							                                             );

							if (shareElem == NULL) {
								// Add cShare to the list
								cShare *share;
								cshare_new((void **)&share);
								cshare_setTicker(share, response.ticker);
								cshare_setValue(share, 0);
								cllist_append(shares, share);
								cshare_destroy(share);

								// Retrieve again the element
								shareElem = cllist_getFirstOcurrenceOfElement(shares,
								lambda (int, (void *elem) {
									cShare *_share = (cShare *)elem;
									if (strcmp(response.ticker, cshare_getTicker(_share)) == 0) {
										return 0;
									}
									return 1;
								})
								                                             );
							}
							// Add the shares
							cshare_setValue(shareElem, cshare_getValue(shareElem) + response.num);

							fdut_msgStdof(buffero, "Purchase made\n");
						} else {
							fdut_msgStdof(buffero, "Purchase could not be made!\n");
							if (strcmp(response.ticker, "unknown") == 0) fdut_msgStdof(buffero, "Ticker unknown\n");
							if (response.total == -1) fdut_msgStdof(buffero, "You don't have enough money\n");
							if (response.num == -1) fdut_msgStdof(buffero, "There are not enough shares\n");
						}

					} else if (result == -1) {
						// ERROR
						msgStdof(buffero, "Oops something went wrong!\n");

					}
				}
				if (rwbytes > 0) prompt();
			}
			break;
			case F_GEK_SELL_M:
			{
				char ticker[216];
				int nShares;
				float revenue;
				cShare *shareElem;

				if (parseSoldFrame(frame_getData(frame), &nShares, &revenue, ticker) == -1) {
					break;
				}

				msgStdof(buffero, "ticker: %s\n", ticker);

				shareElem = cllist_getFirstOcurrenceOfElement(doz_getShares(dozer),
				lambda (int, (void *elem) {
					cShare *share = (cShare *)elem;
					if (strcmp(ticker, cshare_getTicker(share)) == 0) {
						return 0;
					}
					return -1;
				})
				                                             );
				if (shareElem == NULL) {
					msgStdof(buffero, "The server is a traitor -.-, ignoring frame\n");
					break;
				}
				cfloat_set(doz_getMoney(dozer), cfloat_get(doz_getMoney(dozer)) + revenue);
				prompt();
			}
			break;
			case F_DOZ_SELL_D:
			{
				// Retrieve first the number of shares to sell
				regex_t regex;
				int reti;
				size_t nmatch;
				regmatch_t *pmatch;

				char auxbuff[216];
				char ticker[216];
				int numShares;
				cShare *shareElem;

				reti = regcomp(&regex, "^(.+)\n(.+)$", REG_EXTENDED);

				if (reti) {
					msgErrf(buffero, "Error compiling regex, ignoring request\n");
					break;
				}

				nmatch = 3;
				pmatch = (regmatch_t *)malloc(sizeof(regmatch_t) * nmatch);

				reti = regexec(&regex, frame_getData(frame), nmatch, pmatch, 0);
				if (reti == REG_NOMATCH) {
					msgErrf(buffero, "Error matching regex, ignoring request\n");
					frame_destroy(frame);
					regfree(&regex);
					free(pmatch);
					break;
				}

				regex_getRegexString(auxbuff, frame_getData(frame), pmatch, 1);
				strcpy(ticker, auxbuff);

				regex_getRegexString(auxbuff, frame_getData(frame), pmatch, 2);
				numShares = atoi(auxbuff);
				//msgStdof(buffero, "Num shares: %s\n", auxbuff);

				shareElem = cllist_getFirstOcurrenceOfElement(doz_getShares(dozer),
				lambda (int, (void *elem) {
					cShare *share = (cShare *)elem;
					if (strcmp(ticker, cshare_getTicker(share)) == 0) {
						return 0;
					}
					return -1;
				})
				                                             );

				rwbytes = write(*adatac_getSocket(actor), frame, sizeof(Frame));

				if (rwbytes <= 0) {

					msgErrf(buffero, "Disconnected from Gekko, exiting now\n");
					frame_destroy(frame);
					adatac_getPthread(actor)->status = THREAD_NOT_RUNNING;
					handleGekkoDisconnection();
					exitProgram(EXIT_FAILURE);
					pthread_exit(0);

				}

				rwbytes = read(*adatac_getSocket(actor), frame, sizeof(Frame));

				if (rwbytes <= 0) {

					msgErrf(buffero, "Disconnected from Gekko, exiting now\n");
					frame_destroy(frame);
					adatac_getPthread(actor)->status = THREAD_NOT_RUNNING;
					handleGekkoDisconnection();
					exitProgram(EXIT_FAILURE);
					pthread_exit(0);

				}

				if (cf_frame_match(frame) == F_DOZ_SELL_G) {

					cshare_setValue(shareElem, cshare_getValue(shareElem) - numShares);
					msgStdof(buffero, "Commit of sell frame received from the server\n", numShares);

					prompt();
				} else {
					msgStdof(buffero, "Frame not expected, ignoring!\n");
					prompt();
					break;
				}

			}
			break;
			default:
				break;
			}
		}
	}
}

void handleGekkoDisconnection() {
	sockcli.status = SOCK_DISCONNECTED;
	close(sockcli.fd);
}

int parseSoldFrame(char *data, int *nShares, float *revenue, char *ticker)
{
	regex_t regex;
	int reti;
	size_t nmatch;
	regmatch_t *pmatch;

	char auxbuff[216];
	char buffero[512];
	// nShares sold, ticker, total revenue
	reti = regcomp(&regex, "^(.+)\n(.+)\n(.+)$", REG_EXTENDED);

	if (reti) {
		msgErrf(buffero, "Error compiling regex, ignoring request\n");
		return -1;
	}

	nmatch = 4;
	pmatch = (regmatch_t *)malloc(sizeof(regmatch_t) * nmatch);

	reti = regexec(&regex, data, nmatch, pmatch, 0);
	if (reti == REG_NOMATCH) {
		msgErrf(buffero, "Error matching regex, ignoring request\n");
		regfree(&regex);
		free(pmatch);
		return -1;
	}

	regex_getRegexString(auxbuff, data, pmatch, 1);
	*nShares = atoi(auxbuff);

	regex_getRegexString(auxbuff, data, pmatch, 2);
	strcpy(ticker, auxbuff);

	regex_getRegexString(auxbuff, data, pmatch, 3);
	*revenue = atof(auxbuff);

	msgStdof(buffero, "Your shares have been sold! Revenue: %f, ticker: %s, number of shares: %d\n", *revenue, ticker, *nShares);

	return 1;
}

int parseIbexFrame(char *data, LinkedList *ibex35List)
{
	regex_t regex;
	int reti;
	size_t nmatch;
	regmatch_t *pmatch;

	char buffero[512];
	char auxbuff[216];
	IbVal *ibval;

	reti = regcomp(&regex, "^(.+)\n([0-9]+\\.[0-9][0-9]?)?\n([0-9]+)$", REG_EXTENDED);

	if (reti) {
		msgErrf(buffero, "Error compiling regex\n");
		return -1;
	}

	nmatch = 4;
	pmatch = (regmatch_t *)malloc(sizeof(regmatch_t) * nmatch);

	ibval_new((void **)&ibval);

	reti = regexec(&regex, data, nmatch, pmatch, 0);
	if (reti == REG_NOMATCH) {
		msgErrf(buffero, "Error matching regex\n");
		regfree(&regex);
		ibval_destroy(ibval);
		free(pmatch);
		return -1;
	}

	regex_getRegexString(auxbuff, data, pmatch, 1);
	ibval_setTicker(ibval, auxbuff);

	regex_getRegexString(auxbuff, data, pmatch, 2);
	ibval_setValue(ibval, atof(auxbuff));

	regex_getRegexString(auxbuff, data, pmatch, 3);
	ibval_setNShares(ibval, atof(auxbuff));

	llist_append(ibex35List, ibval);

	regfree(&regex);
	free(pmatch);
	free(ibval);

	return 0;
}

int parseBuyFrame(char *data, BuyResponse *response)
{
	regex_t regex;
	int reti;
	size_t nmatch;
	regmatch_t *pmatch;

	char buffero[512];
	char auxbuff[216];

	reti = regcomp(&regex, "^(.+)\n(.+)\n(.+)\n(.+)\n(.+)$", REG_EXTENDED);

	if (reti) {
		msgErrf(buffero, "Error compiling regex\n");
		return -1;
	}

	nmatch = 6;
	pmatch = (regmatch_t *)malloc(sizeof(regmatch_t) * nmatch);

	reti = regexec(&regex, data, nmatch, pmatch, 0);
	if (reti == REG_NOMATCH) {
		msgErrf(buffero, "Error matching regex\n");
		regfree(&regex);
		free(pmatch);
		return -1;
	}

	regex_getRegexString(auxbuff, data, pmatch, 1);
	response->status = atoi(auxbuff);

	regex_getRegexString(auxbuff, data, pmatch, 2);
	strcpy(response->ticker, auxbuff);

	regex_getRegexString(auxbuff, data, pmatch, 3);
	response->num = atoi(auxbuff);

	regex_getRegexString(auxbuff, data, pmatch, 4);
	response->price = atof(auxbuff);

	regex_getRegexString(auxbuff, data, pmatch, 5);
	response->total = atof(auxbuff);

	regfree(&regex);
	free(pmatch);
	return 1;


}


