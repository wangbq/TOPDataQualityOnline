#include <framework/core/Module.h>
#include <topcaf/dataobjects/EventWaveformPacket.h>
#include "TH1F.h"
#include "TH2F.h"
#include "TCanvas.h"
#include <string>
#include <map>
#include <vector>
#include <deque>
#include <utility>

namespace Belle2 {
  class TOPCAFDQMModule : public Module {
	  typedef unsigned long long topcaf_channel_id_t;
  public:
    TOPCAFDQMModule();

    void defineHisto();
    void initialize();
    void beginRun();
    void event();
    void endRun();
    void terminate();
	void makePlots();
    //void drawWaveforms(EventWaveformPacket* ewp);
    //void basicDebuggingPlots(EventWaveformPacket* ewp);

  private:
    int m_iEvent; // keep track of iterations within run
	int m_framesPerEvent;
	std::map<topcaf_channel_id_t, std::vector<double> > m_wf_buffer;
	TCanvas *m_dqm_canvas;
	TH1F *h_channel;
  };

}
