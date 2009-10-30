#define _MAIN_ARCHIVER_CPP_

extern "C" {
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>
}

#include "TnmsArchiveManager.h"
using namespace tnmsdb;

#include "FileUtils.h"
using namespace tcanetpp;


static const
char process[]    = "archiver";


TnmsArchiveManager * mgr = NULL;



void version()
{
    printf("%s version: %s\n", process, TnmsArchiveManager::Version().c_str());
    exit(0);
}


void usage()
{
    printf("Usage: %s [-dDvV] [-c cfg]\n", process);
    printf("          -c <cfg>  :  xml configuration file (required)\n");
    printf("          -d        :  enable debug output\n");
    printf("          -D        :  run as a background daemon\n");
    printf("          -v        :  enable verbose console output when not daemonized\n");
    printf("          -V        :  display version info and exit\n");
    exit(0);
}


void sigHandler ( int signal )
{
    if ( signal == SIGINT || signal == SIGTERM ) {
        if ( mgr )
            mgr->setAlarm();
    } else if ( signal == SIGHUP ) {
        if ( mgr )
            mgr->setHUP();
    } else if ( signal == SIGUSR1 ) {
        if ( mgr )
            mgr->setUSR();
    }

    return;
}


void initDaemon ( const char* pname )
{
    pid_t  pid;

    if ( (pid = fork()) != 0 )
        exit(0);

    setsid();
    signal(SIGHUP, SIG_IGN);

    if ( (pid = fork()) != 0 )
        exit(0);
    chdir("/");
    umask(0);

    for ( int i = 1; i < MAX_FDVAL; i++ )
        close(i);

    return;
}



int main ( int argc, char **argv )
{
    std::string  cfgfile, dtdfile;
    char         optChar;
    char        *cfgstr  = NULL;
    bool         debug   = true;
    bool         verbose = false;
    bool         daemon  = false;
    
    while ( (optChar = getopt(argc, argv, "c:dDvV")) != EOF ) {
        
        switch ( optChar ) {
            case 'c':
              cfgstr = strdup(optarg);
              break;
            case 'd':
              debug = true;
              break;
            case 'D':
              daemon = true;
              break;
            case 'v':
              verbose = true;
              break;
            case 'V':
              version();
              break;
            default:
              usage();
              break;
        }
    }

    if ( cfgstr != NULL && strlen(cfgstr) > 0 ) {
        cfgfile = cfgstr;
        free(cfgstr);
    } else {
        usage();
    }
    
    if ( ! FileUtils::IsReadable(cfgfile.c_str()) )
        usage();

    if ( daemon ) {
        verbose = false;
        initDaemon(process);
    }
    
    signal(SIGPIPE, SIG_IGN);
    signal(SIGINT,  &sigHandler);
    signal(SIGTERM, &sigHandler);
    signal(SIGHUP,  &sigHandler);
    signal(SIGUSR1, &sigHandler);

    mgr = new TnmsArchiveManager(cfgfile);
    mgr->setDebug(debug);
    //mgr->setVerbose(verbose);

    //printf("Starting up...\n");
    // main loop 
    mgr->run();

    // cleanup
    delete mgr;

    return 0;
}
