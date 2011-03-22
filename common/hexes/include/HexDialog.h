#ifndef _HEXES_HEXDIALOG_H_
#define _HEXES_HEXDIALOG_H_

#include <string>

#include "HexPanel.h"


namespace hexes {

class HexDialog : public HexPanel {

  public:

    HexDialog ( const std::string & title,
                const std::string & txt,
                int   height, int   width,
                int   starty, int   startx );

    virtual ~HexDialog();

    virtual int  showDialog();


  protected:

    void  initDialog();


  private:

    std::string  _dialog;

};

} // namespace 
#endif // _HEXES_HEXDIALOG_H_
