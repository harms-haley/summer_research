#include "TChain.h"
#include "TFile.h"
#include "TH1F.h"
#include "TH2F.h"
#include "TCanvas.h"
#include <iostream>
#include "TF1.h"
#include <vector>
#include <cmath>
#include <numeric>
#include <algorithm>
#include "TPaveText.h"
#include "TF2.h"
#include <TStyle.h>
#include <TMath.h>

double gaussian(double *x, double *par) {
	double arg = (x[0] - par[1]) / par[2];
    	return par[0] * exp(-0.5 * arg * arg);
}

double gaussian2D(double *x, double *par) {
	double argX = (x[0] - par[1]) / par[2];
    	double argY = (x[1] - par[3]) / par[4];
    	return par[0] * exp(-0.5 * (argX * argX + argY * argY));
}


double calculate_mean(const std::vector<double>& data) {
	return std::accumulate(data.begin(), data.end(), 0.0) / data.size();
}
double calculate_stddev(const std::vector<double>& data, double mean) {
	double sum = 0.0;
    	for (double value : data) {
        	sum += (value - mean) * (value - mean);
    	}
    	return sqrt(sum / data.size());
}

void fit_gaussian1D(TH1F* histogram) {
	int n_bins = histogram->GetNbinsX();
    	std::vector<double> data;
    	for (int i = 1; i <= n_bins; ++i) {
        	int bin_count = histogram->GetBinContent(i);
        	double bin_center = histogram->GetBinCenter(i);
        	for (int j = 0; j < bin_count; ++j) {
            		data.push_back(bin_center);
        	}
    	}	
	double mean = calculate_mean(data);
    	double stddev = calculate_stddev(data, mean);

    	TF1 *fit_func = new TF1("fit_func", gaussian, histogram->GetXaxis()->GetXmin(), histogram->GetXaxis()->GetXmax(), 3);
    	fit_func->SetParameters(histogram->GetMaximum(), mean, stddev);
    	histogram->Fit(fit_func, "R");
	
	double chi2 = fit_func->GetChisquare();
    	int ndf = fit_func->GetNDF();

	gStyle->SetOptStat(0);
	double entries = histogram->GetEntries();
	std::cout << "Entries: " << entries << std::endl;

	//TPaveText *pt = new TPaveText(0.6, 0.7, 0.9, 0.9, "NDC");
    	//pt->SetFillColor(0);
    	//pt->SetTextAlign(12);
    	//pt->AddText(Form("#chi^{2} = %.2f", chi2));
    	//pt->AddText(Form("NDF = %d", ndf));
    	//pt->Draw();

    	//histogram->GetListOfFunctions()->Add(pt);
}

void fit_gaussian2D(TH2F* histogram) {
	int n_binsX = histogram->GetNbinsX();
    	int n_binsY = histogram->GetNbinsY();
    	std::vector<double> dataX;
    	std::vector<double> dataY;
    	for (int i = 1; i <= n_binsX; ++i) {
        	for (int j = 1; j <= n_binsY; ++j) {
            		int bin_count = histogram->GetBinContent(i, j);
            		double bin_centerX = histogram->GetXaxis()->GetBinCenter(i);
            		double bin_centerY = histogram->GetYaxis()->GetBinCenter(j);
            		for (int k = 0; k < bin_count; ++k) {
                		dataX.push_back(bin_centerX);
                		dataY.push_back(bin_centerY);
            		}
        	}
    	}

	double meanX = calculate_mean(dataX);
	std::cout << "MeanX = " << meanX << std::endl;
    	double meanY = calculate_mean(dataY);
	std::cout << "MeanY = " << meanY << std::endl;
    	double stddevX = calculate_stddev(dataX, meanX);
	std::cout << "stddevX = " << stddevX << std::endl;
    	double stddevY = calculate_stddev(dataY, meanY);
	std::cout << "stddevY = " << stddevY << std::endl;

    	TF2 *fit_func = new TF2("fit_func", gaussian2D, histogram->GetXaxis()->GetXmin(), histogram->GetXaxis()->GetXmax(), histogram->GetYaxis()->GetXmin(), histogram->GetYaxis()->GetXmax(), 5);
    	fit_func->SetParameters(histogram->GetMaximum(), meanX, stddevX, meanY, stddevY);
    	histogram->Fit(fit_func, "R");

    	double chi2 = fit_func->GetChisquare();
    	int ndf = fit_func->GetNDF();

	std::cout << "#chi^{2} = " << chi2 << std::endl;
    	std::cout << "NDF = " << ndf << std::endl;

	gStyle->SetOptStat(0);
	double entries = histogram->GetEntries();
	std::cout << "Entries: " << entries << std::endl;	

	//double mean = (meanX, meanY);
    	//double sigma = (stdevX, stdevY);  // Approximation of sigma for contour purposes
	//double meanX = calculate_mean(dataX);
        //double meanY = calculate_mean(dataY);
        //double stddevX = calculate_stddev(dataX, meanX);
        //double stddevY = calculate_stddev(dataY, meanY);   	
    	std::vector<double> contours = {
        	meanX - 3 * stddevX, meanY - 3 * stddevY, 
        	meanX - 2 * stddevX, meanY - 2 * stddevY, 
        	meanX - 1 * stddevX, meanY - 1 * stddevY, 
        	meanX + 1 * stddevX, meanY + 1 * stddevY,
        	meanX + 2 * stddevX, meanY + 2 * stddevY,
        	meanX + 3 * stddevX, meanY + 3 * stddevY,
	};

	histogram->SetContour(contours.size(), contours.data());


	//histogram3_f_nb->SetContour(contours.size(), contours.data());
	//histogram6_f_tc->SetContour(contours.size(), contours.data());
	//histogramxy->SetContour(contours.size(), contours.data());

	//TPaveText *pt = new TPaveText(0.6, 0.7, 0.9, 0.9, "NDC");
    	//pt->SetFillColor(0);
    	//pt->SetTextAlign(12);
    	//pt->AddText(Form("#chi^{2} = %.2f", chi2));
    	//pt->AddText(Form("NDF = %d", ndf));
    	//pt->Draw();

    	//histogram->GetListOfFunctions()->Add(pt);
}
void AllDataOnlyBeam() {
	TChain chain("crtana/tree"); // Ensure "myTree" is the correct name of your TTree
	chain.Add("/pnfs/sbnd/persistent/users/hlay/crt_comm_summer_2024/run1369*_crtana.root"); // Replace with your file path pattern
	TH1F *histogram1_f_nb = new TH1F("histogram1_f_nb", "Front Face X;No Beam", 10, -400, 400);
	histogram1_f_nb->GetXaxis()->SetTitle("X Location (cm)");
    	histogram1_f_nb->GetYaxis()->SetTitle("Number of Hits");

	TH1F *histogram2_f_nb = new TH1F("histogram2_f_nb", "Front Face Y;No Beam", 10, -400, 400);
	histogram2_f_nb->GetXaxis()->SetTitle("Y Location (cm)");
        histogram2_f_nb->GetYaxis()->SetTitle("Number of Hits");

	TH2F *histogram3_f_nb = new TH2F("histogram3_f_nb", "Front Face XY;No Beam", 10, -400, 400, 10, -400, 400);
	histogram3_f_nb->GetXaxis()->SetTitle("X Location (cm)");
        histogram3_f_nb->GetYaxis()->SetTitle("Y Location (cm)");

	TH1F *histogram4_f_tc = new TH1F("histogram4_f_tc", "Front Face X; Time Cut", 10, -400, 400);
        histogram4_f_tc->GetXaxis()->SetTitle("X Location (cm)");
        histogram4_f_tc->GetYaxis()->SetTitle("Number of Hits");

	TH1F *histogram5_f_tc = new TH1F("histogram5_f_tc", "Front Face Y; Time Cut", 10, -400, 400);
        histogram5_f_tc->GetXaxis()->SetTitle("Y Location (cm)");
        histogram5_f_tc->GetYaxis()->SetTitle("Number of Hits");

	TH2F *histogram6_f_tc = new TH2F("histogram6_f_tc", "Front Face XY; Time Cut", 10, -400, 400, 10, -400, 400);
	histogram6_f_tc->GetXaxis()->SetTitle("X Location (cm)");
        histogram6_f_tc->GetYaxis()->SetTitle("Y Location (cm)");
	histogram6_f_tc->SetContour(contours.size(), contours.data());

    	TCanvas *c1_f_nb = new TCanvas("c1_f_nb", "Front Face X NB");
	TCanvas *c2_f_nb = new TCanvas("c2_f_nb", "Front Face Y NB");
	TCanvas *c3_f_nb = new TCanvas("c3_f_nb", "Front Face XY NB");
	TCanvas *c4_f_tc = new TCanvas("c4_f_tc", "Front Face X TC");
        TCanvas *c5_f_tc = new TCanvas("c5_f_tc", "Front Face Y TC");
        TCanvas *c6_f_tc = new TCanvas("c6_f_tc", "Front Face XY TC");
	TCanvas *c7_f_ob = new TCanvas("c7_f_ob", "Front Face X OB");
        TCanvas *c8_f_ob = new TCanvas("c8_f_ob", "Front Face Y OB");
        TCanvas *c9_f_ob = new TCanvas("c9_f_ob", "Front Face XY OB");

	std::vector<double> *cl_sp_x = nullptr;
    	std::vector<double> *cl_sp_y = nullptr;
    	std::vector<double> *cl_sp_z = nullptr;
    	std::vector<double> *cl_sp_ts1 = nullptr;
    	std::vector<bool> *cl_has_sp = nullptr;
	chain.SetBranchAddress("cl_has_sp", &cl_has_sp);
    	chain.SetBranchAddress("cl_sp_x", &cl_sp_x);
    	chain.SetBranchAddress("cl_sp_y", &cl_sp_y);
    	chain.SetBranchAddress("cl_sp_z", &cl_sp_z);
    	chain.SetBranchAddress("cl_sp_ts1", &cl_sp_ts1);

	Long64_t nEntries = chain.GetEntries();

	for (Long64_t i = 0; i < nEntries; ++i) {
        	chain.GetEntry(i);
		if (cl_has_sp == nullptr || cl_has_sp->size() == 0 || !cl_has_sp->at(0)) {
            		continue;
        	}
		
		for (size_t j = 0; j < cl_sp_x->size(); ++j) {
            		double t1 = cl_sp_ts1->at(j);
            		double x = cl_sp_x->at(j);
            		double y = cl_sp_y->at(j);
            		double z = cl_sp_z->at(j);
			if (i % 10000 == 0) {
				std::cout << "Entry " << i << ", Hit " << j << ": t1=" << t1 << ", x=" << x << ", y=" << y << ", z=" << z << std::endl;
			}
			
			if (1529e3 < t1 && t1 < 1534e3) {
				if (y > -350 && -370 < x && x < 370) {
					if (-200 < z && z < -100) {
						histogram4_f_tc->Fill(x);
						histogram5_f_tc->Fill(y);
						histogram6_f_tc->Fill(x,y);
					}
				}
			}
			
			if (0 < t1 && t1 < 1529e3 || 1534e3 < t1 && t1 < 1000e6) {
				if (y > -350 && -370 < x && x < 370) {  // cut off feet of detector
                    			if (-200 < z && z < -100) {
                        			histogram1_f_nb->Fill(x);
                        			histogram2_f_nb->Fill(y);
                        			histogram3_f_nb->Fill(x, y);
                    			}
                		}
            		}
        	}
    	}
	TH1F *histogramx = (TH1F*)histogram4_f_tc->Clone("histogramx");
	histogramx->Add(histogram1_f_nb, -1.0 / 199999);
	histogramx->GetXaxis()->SetTitle("X Location (cm)");
        histogramx->GetYaxis()->SetTitle("Number of Hits)");

	TH1F *histogramy = (TH1F*)histogram5_f_tc->Clone("histogramy");
        histogramy->Add(histogram2_f_nb, -1.0 / 199999);
	histogramy->GetXaxis()->SetTitle("Y Location (cm)");
        histogramy->GetYaxis()->SetTitle("Number of Hits");

	TH2F *histogramxy = (TH2F*)histogram6_f_tc->Clone("histogramxy");
        histogramxy->Add(histogram3_f_nb, -1.0 / 199999);
	histogramxy->GetXaxis()->SetTitle("X Location (cm)");
        histogramxy->GetYaxis()->SetTitle("Y Location (cm)");
	histogramxy->SetContour(contours.size(), contours.data());

	c1_f_nb->cd();
    	histogram1_f_nb->Draw();
	fit_gaussian1D(histogram1_f_nb);
    	c2_f_nb->cd();
    	histogram2_f_nb->Draw();
	fit_gaussian1D(histogram2_f_nb);
    	c3_f_nb->cd();
    	histogram3_f_nb->Draw("COLZ");
	fit_gaussian2D(histogram3_f_nb);	
	c4_f_tc->cd();
	histogram4_f_tc->Draw();
	fit_gaussian1D(histogram4_f_tc);
	c5_f_tc->cd();
        histogram5_f_tc->Draw();
        fit_gaussian1D(histogram5_f_tc);
	c6_f_tc->cd();
        histogram6_f_tc->Draw("COLZ");
        fit_gaussian2D(histogram6_f_tc);
	c7_f_ob->cd();
        histogramx->Draw();
        fit_gaussian1D(histogramx);
	c8_f_ob->cd();
        histogramy->Draw();
        fit_gaussian1D(histogramy);
	c9_f_ob->cd();
        histogramxy->Draw("COLZ");
        fit_gaussian2D(histogramxy);


	c1_f_nb->SaveAs("Front_face_x_nb.png");
    	c2_f_nb->SaveAs("Front_face_y_nb.png");
    	c3_f_nb->SaveAs("Front_face_nb.png");
	c4_f_tc->SaveAs("Front_face_x_tc.png");
        c5_f_tc->SaveAs("Front_face_y_tc.png");
        c6_f_tc->SaveAs("Front_face_tc.png");
	c7_f_ob->SaveAs("Front_face_x_ob.png");
        c8_f_ob->SaveAs("Front_face_y_ob.png");
        c9_f_ob->SaveAs("Front_face_ob.png");

}

