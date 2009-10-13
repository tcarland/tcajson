#ifndef _TNMSDB_ARCHIVECLIENT_H_
#define _TNMSDB_ARCHIVECLIENT_H_

#include "ArchiveConfig.h"
#include "TnmsArchiveManager.h"

#include "TnmsClient.h"


namespace tnmsdb {

class ArchiverThread;


class ArchiveClient : public tnmsCore::TnmsClient {

  public:

    ArchiveClient ( ArchiverSet & archivers, TnmsClientConfig & cfg );

    virtual ~ArchiveClient();


    void     init();
    void     setSubscribed ( bool sub );


  protected:
    
    ArchiverSet        _archivers;
    
    TnmsClientConfig   _config;

};

}  // namespace


#endif  // _TNMSDB_ARCHIVECLIENT_H_
