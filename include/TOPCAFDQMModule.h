#include <framework/core/Module.h>
#include <topcaf/dataobjects/EventWaveformPacket.h>
#include "TH1F.h"
#include "TH2F.h"
#include "TCanvas.h"
#include "TMultiGraph.h"
#include <string>
#include <set>
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
        void clear_graph();
        void update_graph();
        void drawWaveforms(EventWaveformPacket* ewp);
        //void basicDebuggingPlots(EventWaveformPacket* ewp);

    private:
        int m_iEvent;
        int m_refreshCount;
        TCanvas* m_canvas[4];
        std::map<int, std::map<int, TMultiGraph*>> m_channels;
        std::map<int, std::map<int, std::set<int>>> m_channelLabels;
    };

}
