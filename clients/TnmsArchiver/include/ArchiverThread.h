#ifndef _TNMSARCHIVE_ARCHIVERTHREAD_H_
#define _TNMSARCHIVE_ARCHIVERTHREAD_H_

#include <set>

#include "TnmsTree.h"
#include "TnmsMetric.h"
using namespace tnmsCore;


#include "Thread.h"
#include "ThreadLock.h"
#include "Queue.hpp"
using namespace tcanetpp;

namespace tnmsArchive {


class ArchiverThread : public tcanetpp::Thread {

  public:

    ArchiverThread();
    virtual ~ArchiverThread();


    void        run();
    void        notify();


  private:

    tnmsCore::TnmsTree*           _tree;

    tcanetpp::ThreadLock*         _lock;

    tcanetpp::Queue<TnmsMetric>   _updateQueue;
    tcanetpp::Queue<std::string>  _removeQueue;

};


} // namespace

#endif  // _TNMSARCHIVE_ARCHIVERTHREAD_H_

