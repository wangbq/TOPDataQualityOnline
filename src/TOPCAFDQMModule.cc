#include <framework/core/HistoModule.h>
#include <topcaf/modules/TOPCAFDQM/TOPCAFDQMModule.h>
#include <framework/datastore/StoreObjPtr.h>
#include <framework/datastore/StoreArray.h>
#include <framework/pcore/RbTuple.h>
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

		TOPCAFDQMModule::TOPCAFDQMModule() : Module(), m_iEvent(0)
	{
		setDescription("TOPCAF online monitoring module");
		addParam("refreshCount", m_refreshCount, "refresh count", 10);
	}


	void TOPCAFDQMModule::defineHisto()
	{
	}


	void TOPCAFDQMModule::initialize()
	{
		// Register histograms (calls back defineHisto)
		//REG_HISTOGRAM;
	}


	void TOPCAFDQMModule::beginRun()
	{
		//m_DRAWWAVES = true;
		//m_DEBUGGING = true;
		m_dqm_canvas = new TCanvas("dqm_canvas","DQM Canvas",800,600);
		h_channel = new TH1F("channel","channel", 512, 0.5, 512.5);
		h_channel->Draw();
		m_dqm_canvas->Show();
		//cout<<"DISPLAY: "<<getenv("DISPLAY")<<endl;
		//usleep(1000000);
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

	void TOPCAFDQMModule::makePlots() {
		if (m_wf_buffer.size()==0) return;
		//cout<<"start makePlots"<<endl;
		for (auto it=m_wf_buffer.begin(); it!=m_wf_buffer.end(); ++it) {
			h_channel->Fill((it->first) % 512);
			//h_channel->Fill(256);
		}
		//cout<<"after loop"<<endl;
		if (m_iEvent % m_refreshCount == 0) {
			//cout<<"-------------------------------------------------------------------------------------------------------"<<endl;
			//cout<<"total entries: "<<m_iEvent<<endl;
			m_dqm_canvas->GetPad(0)->Modified();
			m_dqm_canvas->Update();
			//h_channel->Draw();
			//usleep(1000000);
		}
		//cout<<"end makePlots"<<endl;
	}

	void TOPCAFDQMModule::event() {
		//Get Waveform from datastore
		StoreArray<EventWaveformPacket> evtwaves_ptr;
		evtwaves_ptr.isRequired();
		if (not evtwaves_ptr) {
			return;
		}
		for (int c = 0; c < evtwaves_ptr.getEntries(); c++) {
			EventWaveformPacket* evtwave_ptr = evtwaves_ptr[c];
			unsigned int channelID = evtwave_ptr->GetChannelID();
			//if (channelID>512) cout<<"wrong channelID: "<<channelID<<endl;
			const vector<double> v_samples = evtwave_ptr->GetSamples();
			//size_t nsamples = v_samples.size();
			m_wf_buffer.insert(make_pair(channelID, v_samples));
		}
		m_iEvent += 1;
		makePlots();
		m_wf_buffer.clear();// clear wf buffer for next event
		return;
	}


	void TOPCAFDQMModule::endRun()
	{
	}


	void TOPCAFDQMModule::terminate()
	{
	}
}
