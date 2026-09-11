#include "Connection.h"
#include "Publish.h"
#include "Subscribe.h"
#include "k.h"

extern "C" {

EXP K1(kexport) {
  K sv=ktn(KS,0),fv=ktn(0,0);
  #define _(s,a) js(&sv,ss((S)#s));jk(&fv,dl((V*)kamps_##s,a));
  _(create,3)_(connect,1)_(disconnect,1)_(isConnected,1)
  _(publishBinary,4)_(publishBinaryAt,5)
  _(subscribe,5)_(discard,3)_(unsubscribe,2)
  _(init,1)
  R xD(sv, fv);
}

}