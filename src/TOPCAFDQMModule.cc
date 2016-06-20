#include <framework/core/HistoModule.h>
#include <topcaf/modules/TOPCAFDQM/TOPCAFDQMModule.h>
#include <framework/datastore/StoreObjPtr.h>
#include <framework/datastore/StoreArray.h>
#include <framework/pcore/RbTuple.h>
#include <topcaf/dataobjects/TopConfigurations.h>
#include <utility>
#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include "TDirectory.h"
#include "TCanvas.h"
#include "TGraph.h"

using namespace std;

namespace Belle2 {
	REG_MODULE(TOPCAFDQM)

    TOPCAFDQMModule::TOPCAFDQMModule() : Module(), m_iEvent(0) {
		setDescription("TOPCAF online monitoring module");
		addParam("refreshCount", m_refreshCount, "refresh count", 1);
		addParam("framesPerEvent", m_framesPerEvent, "frames per event", 16);
	}


	void TOPCAFDQMModule::defineHisto() {
	}

	void TOPCAFDQMModule::initialize() {
	}

	void TOPCAFDQMModule::beginRun() {
	}

    void TOPCAFDQMModule::clear_graph() {
        for (auto scrod_it : m_channels) {
            int scrod_id = scrod_it.first;
            for (auto graph_it : scrod_it.second) {
                TMultiGraph* mg = graph_it.second;
                delete mg;
            }
            m_channels[scrod_id].clear();
			m_channelLabels[scrod_id].clear();
        }
    }

    void TOPCAFDQMModule::update_graph() {
        for (auto scrod_it : m_channels) {
            int scrod_id = scrod_it.first;
			if (m_canvas.find(scrod_id) == m_canvas.end()) {
				string cname = string("scrod_id_") + to_string(scrod_id);
				m_canvas[scrod_id] = new TCanvas(cname.c_str(), cname.c_str(),800,800);
				m_canvas[scrod_id]->Divide(4,4);
				m_canvas[scrod_id]->Show();
			}
            for (auto graph_it : scrod_it.second) {
                int asic_id = graph_it.first;
                TMultiGraph* mg = graph_it.second;
                m_canvas[scrod_id]->cd(asic_id+1);
                mg->Draw("ALP");
                m_canvas[scrod_id]->GetPad(asic_id+1)->Modified();
            }
			m_canvas[scrod_id]->Update();
        }
    }

	void TOPCAFDQMModule::drawWaveforms(EventWaveformPacket* ewp) {
		const EventWaveformPacket& v = *ewp;
		vector<double> y = v.GetSamples();
		if (y.empty()) {
			return;
		}
		int scrodid = v.GetScrodID();
		if (scrodid == 0) {
			return;
		}
		int asicid = v.GetASICRow() + 4 * v.GetASICColumn();
		if (m_channelLabels[scrodid].find(asicid) == m_channelLabels[scrodid].end()) {
			string gname = string("channels") + to_string(scrodid) + string("_") + to_string(asicid);
			m_channels[scrodid].insert(make_pair(asicid, new TMultiGraph(gname.c_str(), gname.c_str())));
		}
		int iChannel = v.GetASICChannel();
		if (m_channelLabels[scrodid][asicid].find(iChannel) != m_channelLabels[scrodid][asicid].end()) {
			return;
		}
		m_channelLabels[scrodid][asicid].insert(iChannel);
		TMultiGraph* mg = m_channels[scrodid][asicid];

		vector<double> x;
		for (size_t i = 0; i < y.size(); ++i) {
			y[i] += iChannel * 1000;
			x.push_back(i);
		}
		TGraph* g = new TGraph(y.size(), &x[0], &y[0]);
		g->SetMarkerStyle(7);
		mg->Add(g);
	}

	void TOPCAFDQMModule::event() {
		StoreArray<EventWaveformPacket> evtwaves_ptr;
		evtwaves_ptr.isRequired();
		if (not evtwaves_ptr) {
			return;
		}
		for (int c = 0; c < evtwaves_ptr.getEntries(); c++) {
			EventWaveformPacket* evtwave_ptr = evtwaves_ptr[c];
			drawWaveforms(evtwave_ptr);
		}
		m_iFrame += 1;
		if (m_iFrame % m_framesPerEvent == 0) {
			m_iEvent += 1;
			if (m_iEvent % m_refreshCount == 0) {
				update_graph();
			}
			clear_graph();
		}
		return;
	}


	void TOPCAFDQMModule::endRun() {
	}


	void TOPCAFDQMModule::terminate() {
	}
}
