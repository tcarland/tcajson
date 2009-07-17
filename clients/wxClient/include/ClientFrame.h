#ifndef _CLIENTFRAME_H_
#define _CLIENTFRAME_H_

#include <wx/wx.h>

#include "TnmsWxTree.h"



class ClientFrame : public wxFrame {

  public:

    ClientFrame ( const wxString & title, 
                  TnmsTree_R     & tree );


    void        OnSelect        ( wxCommandEvent & event );
    void        OnQuit          ( wxCommandEvent & event );


  protected:

    void        initMenuBar();


  private:

    TnmsTree_R &        _stree;

    wxPanel*            _mainPanel;
    wxMenuBar*          _menuBar;
    wxMenu*             _menuFile;

    TnmsWxTree*         _tree;

    wxGenericDirCtrl*   _gdir;
    wxListCtrl*         _lCtrl1;
    wxListCtrl*         _lCtrl2;

};


#endif //_CLIENTFRAME_H_

