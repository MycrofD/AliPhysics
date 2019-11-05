#include "TList.h"
#include "TMatrixD.h"
#include "TParticle.h"
#include "TMath.h"
#include "TRandom3.h"
#include "TVector3.h"
#include "TGraph.h"
#include "TFile.h"
#include "TVector3.h"
#include "TLorentzVector.h"
#include "Math/SVector.h"
#include "Math/SMatrix.h"
#include "TCanvas.h"
#include "TPaveText.h"
#include "AliEmcalJet.h"
#include "AliParticleContainer.h"
#include "AliAnalysisUtils.h"
#include "AliExternalTrackParam.h"
#include "AliAODEvent.h"
#include "AliAODTrack.h"
#include "AliAODVertex.h"
#include "AliAODPidHF.h"
#include "AliAODMCParticle.h"
#include "AliAODTracklets.h"
#include "AliMCEvent.h"
#include "AliESDEvent.h"
#include "AliESDUtils.h"
#include "AliMCEventHandler.h"
#include "AliStack.h"
#include "AliPIDResponse.h"
#include "AliLog.h"
#include "AliAnalysisManager.h"
#include "AliInputEventHandler.h"
#include "AliVEventHandler.h"
#include "AliVVertex.h"
#include "AliVParticle.h"
#include "AliAODMCHeader.h"
#include "AliJetContainer.h"
#include "AliGenEventHeader.h"
#include "AliVertexerTracks.h"
#include "AliEmcalList.h"
#include "THnSparse.h"
#include "AliAnalysisTaskEmcalJet.h"


//***********************************//
#include "AliAnalysisTaskHFJetIPQA.h"
//***********************************//
#include <vector>
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>
#include "AliAnalysisHelperJetTasks.h"
#include "AliGenPythiaEventHeader.h"
#include "TChain.h"
#include <map>
using std::min;
using std::cout;
using std::endl;
using std::vector;
using std::pair;
using std::map;
ClassImp(AliAnalysisTaskHFJetIPQA)

AliAnalysisTaskHFJetIPQA::AliAnalysisTaskHFJetIPQA():
AliAnalysisTaskEmcalJet(),
fEventCuts(0),
fHistManager(),
fEventVertex(nullptr),
fPidResponse(nullptr),
fRunSmearing(kFALSE),
fUsePIDJetProb(kFALSE),
fDoMCCorrection(kFALSE),
fDoUnderlyingEventSub(kFALSE),
fDoFlavourMatching(kFALSE),
fParam_Smear_Sigma(1.),
fParam_Smear_Mean(0.),
fGlobalVertex(kFALSE),
fDoNotCheckIsPhysicalPrimary(kFALSE),
fDoJetProb(kFALSE),
fFillCorrelations(kFALSE),
fDoLundPlane(kFALSE),
fDoTCTagging(kFALSE),
fDoProbTagging(kFALSE),
kTagLevel(3),
fFracs(0),
fXsectionWeightingFactor(1),
fProductionNumberPtHard(-1),
fNThresholds(1),
fJetRadius(0.4),
fDaughtersRadius(1),
fNoJetConstituents(0),
fGraphMean(nullptr),
fGraphSigmaData(nullptr),
fGraphSigmaMC(nullptr),
fGraphXi(nullptr),
fGraphOmega(nullptr),
fK0Star(nullptr),
fPhi(nullptr),
fGeant3FlukaProton(nullptr),
fGeant3FlukaAntiProton(nullptr),
fGeant3FlukaLambda(nullptr),
fGeant3FlukaAntiLambda(nullptr),
fGeant3FlukaKMinus(nullptr),
h1DThresholdsFirst(0),
h1DThresholdsSecond(0),
h1DThresholdsThird(0),
h2DProbLookup(0),
h2DProbDistsUnid(0),
h2DProbDistsudsg(0),
h2DProbDistsc(0),
h2DProbDistsb(0),
h2DProbDistss(0),
h2DProbDists(0),
h2DLNProbDistsUnid(0),
h2DLNProbDistsudsg(0),
h2DLNProbDistsc(0),
h2DLNProbDistsb(0),
h2DLNProbDistss(0),
h2DLNProbDists(0),
cCuts(0),
fh1DCutInclusive(0),
fh1dCutudg(0),
fh1dCutc(0),
fh1dCutb(0),
fh1dCuts(0),
fh1dTracksAccepeted(0),
fh1dCutsPrinted(0),
fHLundIterative(nullptr),
fMCArray(nullptr),
fMCEvent(nullptr),
fESDTrackCut(nullptr),
fVertexer(nullptr),
fMcEvtSampled(kFALSE),
fBackgroundFactorLinus{0},
fPUdsgJet(100),fPSJet(100),fPCJet(100),fPBJet(100),
fJetCont(10),
fAnalysisCuts{0},
fCombined(nullptr),
fMCglobalDCAxyShift(0.0008),
fMCglobalDCASmear(1),
fVertexRecalcMinPt(1.0),
fHardCutOff(0),
fn1_mix(-999.),
fn2_mix(-999.),
fn3_mix(-999.),
fIsMixSignalReady_n1(kFALSE),
fIsMixSignalReady_n2(kFALSE),
fIsMixSignalReady_n3(kFALSE),
fIsSameEvent_n1(kFALSE),
fIsSameEvent_n2(kFALSE),
fIsSameEvent_n3(kFALSE),
fUseTreeForCorrelations(kFALSE),
fCorrelationCrossCheck(nullptr),
fTREE_n1(-99.),
fTREE_n2(-99.),
fTREE_n3(-99.),
fTREE_pt(-1.)

{
    SetMakeGeneralHistograms(kTRUE);
    SetDefaultAnalysisCuts();
    SetNeedEmcalGeom(kFALSE);
    SetOffTrigger(AliVEvent::kINT7);
    SetVzRange(-10,10);
    DefineOutput(1,  AliEmcalList::Class()) ;
}
AliAnalysisTaskHFJetIPQA::AliAnalysisTaskHFJetIPQA(const char *name):
AliAnalysisTaskEmcalJet(name, kTRUE),
fEventCuts(0),
fHistManager(name),
fEventVertex(nullptr),
fPidResponse(nullptr),
fRunSmearing(kFALSE),
fUsePIDJetProb(kFALSE),
fDoMCCorrection(kFALSE),
fDoUnderlyingEventSub(kFALSE),
fDoFlavourMatching(kFALSE),
fParam_Smear_Sigma(1.),
fParam_Smear_Mean(0.),
fGlobalVertex(kFALSE),
fDoNotCheckIsPhysicalPrimary(kFALSE),
fDoJetProb(kFALSE),
fFillCorrelations(kFALSE),
fDoLundPlane(kFALSE),
fDoTCTagging(kFALSE),
fDoProbTagging(kFALSE),
kTagLevel(3),
fFracs(0),
fXsectionWeightingFactor(1.),
fProductionNumberPtHard(-1),
fNThresholds(1),
fJetRadius(0.4),
fDaughtersRadius(1),
fNoJetConstituents(0),
fGraphMean(nullptr),
fGraphSigmaData(nullptr),
fGraphSigmaMC(nullptr),
fGraphXi(nullptr),
fGraphOmega(nullptr),
fK0Star(nullptr),
fPhi(nullptr),
fGeant3FlukaProton(nullptr),
fGeant3FlukaAntiProton(nullptr),
fGeant3FlukaLambda(nullptr),
fGeant3FlukaAntiLambda(nullptr),
fGeant3FlukaKMinus(nullptr),
h1DThresholdsFirst(0),
h1DThresholdsSecond(0),
h1DThresholdsThird(0),
h2DProbLookup(0),
h2DProbDistsUnid(0),
h2DProbDistsudsg(0),
h2DProbDistsc(0),
h2DProbDistsb(0),
h2DProbDistss(0),
h2DProbDists(0),
h2DLNProbDistsUnid(0),
h2DLNProbDistsudsg(0),
h2DLNProbDistsc(0),
h2DLNProbDistsb(0),
h2DLNProbDistss(0),
h2DLNProbDists(0),
cCuts(0),
fh1DCutInclusive(0),
fh1dCutudg(0),
fh1dCutc(0),
fh1dCutb(0),
fh1dCuts(0),
fh1dTracksAccepeted(0),
fh1dCutsPrinted(0),
fHLundIterative(nullptr),
fMCArray(nullptr),
fMCEvent(nullptr),
fESDTrackCut(nullptr),
fVertexer(nullptr),
fMcEvtSampled(kFALSE),
fBackgroundFactorLinus{0},
fPUdsgJet(100),fPSJet(100),fPCJet(100),fPBJet(100),
fJetCont(10),
fAnalysisCuts{0},
fCombined(nullptr),
fMCglobalDCAxyShift(0.000668),
fMCglobalDCASmear(1),
fVertexRecalcMinPt(1.0),
fHardCutOff(0),
fn1_mix(-999.),
fn2_mix(-999.),
fn3_mix(-999.),
fIsMixSignalReady_n1(kFALSE),
fIsMixSignalReady_n2(kFALSE),
fIsMixSignalReady_n3(kFALSE),
fIsSameEvent_n1(kFALSE),
fIsSameEvent_n2(kFALSE),
fIsSameEvent_n3(kFALSE),
fUseTreeForCorrelations(kFALSE),
fCorrelationCrossCheck(nullptr),
fTREE_n1(-99.),
fTREE_n2(-99.),
fTREE_n3(-99.),
fTREE_pt(-1.)
{
    SetNeedEmcalGeom(kFALSE);
    SetOffTrigger(AliVEvent::kINT7);
    SetVzRange(-10,10);
    SetMakeGeneralHistograms(kTRUE);
    SetDefaultAnalysisCuts();
    DefineOutput(1,  AliEmcalList::Class()) ;
}

/*! \brief ChangeDefaultCutTo
 *
 *
 * Modify default analysis cuts
 */
void AliAnalysisTaskHFJetIPQA::ChangeDefaultCutTo(AliAnalysisTaskHFJetIPQA::bCuts cutname, Double_t newcutvalue){
    fAnalysisCuts[cutname] =newcutvalue;
}
/*! \brief SetDefaultAnalysisCuts
 *
 *
 * Set default analysis cuts
 */
void AliAnalysisTaskHFJetIPQA::SetDefaultAnalysisCuts(){
    //DCA
    fAnalysisCuts[bAnalysisCut_DCAJetTrack]     = 0.07;
    fAnalysisCuts[bAnalysisCut_MaxDecayLength]  = 5.;
    fAnalysisCuts[bAnalysisCut_MaxDCA_XY]       = 1.;
    fAnalysisCuts[bAnalysisCut_MaxDCA_Z]        = 2.;

    //Vertex
    fAnalysisCuts[bAnalysisCut_NContibutors]    = 3 ;
    //fAnalysisCuts[bAnalysisCut_RelError_Y]      = 0.2;
    //fAnalysisCuts[bAnalysisCut_RelError_Z]      = 0.2;
    //fAnalysisCuts[bAnalysisCut_Sigma_Y]         = 0.3;
    //fAnalysisCuts[bAnalysisCut_Sigma_Z]         = 0.3;
    //fAnalysisCuts[bAnalysisCut_SigmaDiamond]    = 2.;
    fAnalysisCuts[bAnalysisCut_MaxVtxZ]         = 10.;
    fAnalysisCuts[bAnalysisCut_Z_Chi2perNDF]    =3.5*3.5;
    fAnalysisCuts[bAnalysisCut_MinNewVertexContrib] = 1;

    //Tracks
    fAnalysisCuts[bAnalysisCut_MinTrackPt]      =.5;
    //fAnalysisCuts[bAnalysisCut_MinTrackPtMC]    =.5;
    fAnalysisCuts[bAnalysisCut_MinTPCClus]      =100;
    fAnalysisCuts[bAnalysisCut_MinITSLayersHit] =4;
    fAnalysisCuts[bAnalysisCut_MinTrackChi2] =5;
    fAnalysisCuts[bAnalysisCut_HasSDD]=1;
    fAnalysisCuts[bAnalysisCut_HasTPCrefit]=1;
    fAnalysisCuts[bAnalysisCut_HasITSrefit]=1;
    fAnalysisCuts[bAnalysisCut_KinkCand]=1;

    //Jet Cuts
    fAnalysisCuts[bAnalysisCut_MinJetPt]        =0;  //only for settings output. Not really used as cuts are done in .C file
    fAnalysisCuts[bAnalysisCut_MaxJetPt]        =1000;
    fAnalysisCuts[bAnalysisCut_MinJetEta]       =-0.9;
    fAnalysisCuts[bAnalysisCut_MaxJetEta]       =0.9;

    //Events
    fAnalysisCuts[bAnalysisCut_PtHardAndJetPtFactor] =3;
}

void AliAnalysisTaskHFJetIPQA::SmearTrack(AliAODTrack *track) {
    if(!fIsPythia) return;
    printf("Run Track Smearing.\n");
    // Get reconstructed track parameters
    AliExternalTrackParam et; et.CopyFromVTrack(track);
    Double_t *param=const_cast<Double_t*>(et.GetParameter());
   // Double_t *covar=const_cast<Double_t*>(et.GetCovariance());
    // Get MC info
    Int_t imc=track->GetLabel();
    if (imc<=0) return;
    const AliAODMCParticle *mc=static_cast<AliAODMCParticle*>(fMCArray->At(imc));
    Double_t mcx[3];
    Double_t mcp[3];
    Double_t mccv[36]={0.};
    Short_t  mcc;
    mc->XvYvZv(mcx);
    mc->PxPyPz(mcp);
    mcc=mc->Charge();
    AliExternalTrackParam mct(mcx,mcp,mccv,mcc);
    const Double_t *parammc=mct.GetParameter();
    AliVertex vtx(mcx,1.,1);
    // Correct reference points and frames according to MC
    // TODO: B-Field correct?
    // TODO: failing propagation....
    et.PropagateToDCA(&vtx,track->GetBz(),10.);
    et.Rotate(mct.GetAlpha());
    // Select appropriate smearing functions
    Double_t sd0rpn=fParam_Smear_Sigma;
    Double_t sd0mrpn=fParam_Smear_Mean;//mu m
    Double_t sd0zn =1.;
    Double_t spt1n =1.;
    Double_t sd0rpo=1;
    Double_t sd0mrpo=0;
    Double_t sd0zo =1.;
    Double_t spt1o =1.;
    // Use the same units (i.e. cm and GeV/c)! TODO: pt!
    sd0rpo*=1.e-4;
    sd0zo *=1.e-4;
    sd0rpn*=1.e-4;
    sd0zn *=1.e-4;
    sd0mrpo*=1.e-4;
    sd0mrpn*=1.e-4;
    // Apply the smearing
    Double_t d0zo  =param  [1];
    Double_t d0zmc =parammc[1];
    Double_t d0rpo =param  [0];
    Double_t d0rpmc=parammc[0];
    Double_t pt1o  =param  [4];
    Double_t pt1mc =parammc[4];
    Double_t dd0zo =d0zo-d0zmc;
    Double_t dd0zn =dd0zo *(sd0zo >0. ? (sd0zn /sd0zo ) : 1.);
    Double_t d0zn  =d0zmc+dd0zn;
    Double_t dd0rpo=d0rpo-d0rpmc;
    Double_t dd0rpn=dd0rpo*(sd0rpo>0. ? (sd0rpn/sd0rpo) : 1.);
    Double_t dd0mrpn=sd0mrpn-sd0mrpo;
    Double_t d0rpn =d0rpmc+dd0rpn-dd0mrpn;
    Double_t dpt1o =pt1o-pt1mc;
    Double_t dpt1n =dpt1o *(spt1o >0. ? (spt1n /spt1o ) : 1.);
    Double_t pt1n  =pt1mc+dpt1n;
    param[0]=d0rpn;
    param[1]=d0zn ;
    param[4]=pt1n ;
    // Copy the smeared parameters to the AOD track
    Double_t x[3];
    Double_t p[3];
    et.GetXYZ(x);
    et.GetPxPyPz(p);
    Double_t cv[21];
    et.GetCovarianceXYZPxPyPz(cv);
    track->SetPosition(x,kFALSE);
    track->SetP(p,kTRUE);
    track->SetCovMatrix(cv);
    // Mark the track as "improved" with a trick (this is done with a trick using layer 7 (ie the 8th))
    UChar_t itsClusterMap = track->GetITSClusterMap();
    SETBIT(itsClusterMap,7);
    track->SetITSClusterMap(itsClusterMap);
}


int AliAnalysisTaskHFJetIPQA::GetMCTruth(AliAODTrack * track, int &motherpdg){
    if(!fIsPythia) return 0;
    int pdg = 0;
    AliAODMCParticle *pMCAOD = nullptr;
    if(track->GetLabel()< 0) return pdg;
    pMCAOD = static_cast<AliAODMCParticle*>(fMCArray->At(track->GetLabel()));
    if(!(pMCAOD))  return pdg;
    pdg = pMCAOD->PdgCode();
    motherpdg=0;
    AliAODMCParticle *pMCAODmother = nullptr;
    pMCAODmother = static_cast<AliAODMCParticle*>(fMCArray->At(pMCAOD->GetMother()));
    if(!(pMCAOD))  return pdg;
    motherpdg =pMCAODmother->PdgCode();
    return pdg;
}



Bool_t AliAnalysisTaskHFJetIPQA::FillTrackHistograms(AliVTrack *track, double *dca, double *cov, double weight)
{
    FillHist("fh2dTracksImpParXY",GetValImpactParameter(kXY,dca,cov),track->Pt(),1);     //*this->fXsectionWeightingFactor );
    FillHist("fh2dTracksImpParXYZ",GetValImpactParameter(kXYZ,dca,cov),track->Pt(),1);     //*this->fXsectionWeightingFactor );
    FillHist("fh2dTracksImpParZ",dca[1],track->Pt(),1);     //*this->fXsectionWeightingFactor );
    FillHist("fh2dTracksImpParXYSignificance",GetValImpactParameter(kXYSig,dca,cov),track->Pt(),1);     //*this->fXsectionWeightingFactor );
    //FillHist("fh2dTracksImpParXYZSignificance",GetValImpactParameter(kXYZSig,dca,cov),track->Pt(),1);     //*this->fXsectionWeightingFactor );
    FillHist("fh2dTracksImpParZSignificance",GetValImpactParameter(kZSig,dca,cov),track->Pt(),1);     //*this->fXsectionWeightingFactor );
    FillHist("fh1dTracksImpParXY",GetValImpactParameter(kXY,dca,cov),1);     //*this->fXsectionWeightingFactor );
    FillHist("fh1dTracksImpParXYZ",GetValImpactParameter(kXYZ,dca,cov),1);     //*this->fXsectionWeightingFactor );
    FillHist("fh1dTracksImpParXYSignificance",GetValImpactParameter(kXYSig,dca,cov),1);     //*this->fXsectionWeightingFactor );
    //FillHist("fh1dTracksImpParXYZSignificance",GetValImpactParameter(kXYZSig,dca,cov),1);     //*this->fXsectionWeightingFactor );
    if(fIsPythia){
        FillHist("fh1dTracksImpParXY_McCorr",GetValImpactParameter(kXY,dca,cov),weight);     //*this->fXsectionWeightingFactor );
        FillHist("fh1dTracksImpParXYZ_McCorr",GetValImpactParameter(kXYZ,dca,cov),weight);     //*this->fXsectionWeightingFactor );
        FillHist("fh1dTracksImpParXYSignificance_McCorr",GetValImpactParameter(kXYSig,dca,cov),weight);     //*this->fXsectionWeightingFactor );
        //FillHist("fh1dTracksImpParXYZSignificance_McCorr",GetValImpactParameter(kXYZSig,dca,cov),weight);     //*this->fXsectionWeightingFactor );
        FillHist("fh2dTracksImpParXY_McCorr",GetValImpactParameter(kXY,dca,cov),track->Pt(),weight);     //*this->fXsectionWeightingFactor );
        FillHist("fh2dTracksImpParXYZ_McCorr",GetValImpactParameter(kXYZ,dca,cov),track->Pt(),weight);     //*this->fXsectionWeightingFactor );
        //FillHist("fh2dTracksImpParXYZSignificance_McCorr",GetValImpactParameter(kXYZSig,dca,cov),track->Pt(),weight);     //*this->fXsectionWeightingFactor );
    }
    return kTRUE;
}

/*! \brief Transforms local to global coordinates
 *
 *
 * Transforms local to global coordinates
 */
void AliAnalysisTaskHFJetIPQA::localtoglobal(Double_t alpha ,Double_t* local,Double_t* global)
{
    global[0] = local[0]*  TMath::Sin(alpha) + local[1] * TMath::Cos(alpha);
    global[1] = local[0]*  TMath::Cos(alpha) + local[1] * TMath::Sin(-alpha);
    global[2] = local[2];
    return;
}
/*! \brief Cleanup
 *
 *
 * Cleanup of the event-wise globals
 */
/*void AliAnalysisTaskHFJetIPQA::EventwiseCleanup(){
    fEtaBEvt.clear();
    fPhiBEvt.clear();
    fEtaCEvt.clear();
    fPhiCEvt.clear();
    fEtaUdsgEvt.clear();
    fPhiUdsgEvt.clear();
    fEtaSEvt.clear();
    fPhiSEvt.clear();
    fMcEvtSampled = kFALSE;
}
*/

void AliAnalysisTaskHFJetIPQA::FillRecHistograms(int jetflavour, double jetpt, double eta, double phi){
  FillHist("fh1dJetRecPt",jetpt, 1);  //this->fXsectionWeightingFactor );
  FillHist("fh1dJetRecEtaPhiAccepted",eta,phi, 1);   //this->fXsectionWeightingFactor );
  FillHist("fh1dJetRecPtAccepted",jetpt, 1);  //this->fXsectionWeightingFactor );

  if(fIsPythia){
    if(jetflavour==0)     FillHist("fh1dJetRecPtUnidentified",jetpt, 1);    //this->fXsectionWeightingFactor );
      else if(jetflavour==1)FillHist("fh1dJetRecPtudsg",        jetpt, 1);    //this->fXsectionWeightingFactor );
      else if(jetflavour==2)FillHist("fh1dJetRecPtc",           jetpt, 1);    //this->fXsectionWeightingFactor );
      else if(jetflavour==3)FillHist("fh1dJetRecPtb",           jetpt, 1);    //this->fXsectionWeightingFactor );
      else if(jetflavour==4)FillHist("fh1dJetRecPts",           jetpt, 1);    //this->fXsectionWeightingFactor );
  }
}

void AliAnalysisTaskHFJetIPQA::FillGenHistograms(int jetflavour, AliEmcalJet* jetgen){
    FillHist("fh1dJetGenPt",GetPtCorrectedMC(jetgen), 1); //this->fXsectionWeightingFactor);
    if(jetflavour ==0)      FillHist("fh1dJetGenPtUnidentified",GetPtCorrectedMC(jetgen), 1); // this->fXsectionWeightingFactor );
    else if(jetflavour ==1) FillHist("fh1dJetGenPtudsg",GetPtCorrectedMC(jetgen), 1);   //this->fXsectionWeightingFactor );
    else if(jetflavour ==2) FillHist("fh1dJetGenPtc",GetPtCorrectedMC(jetgen), 1);  //this->fXsectionWeightingFactor );
    else if(jetflavour ==3) FillHist("fh1dJetGenPtb",GetPtCorrectedMC(jetgen), 1);  //this->fXsectionWeightingFactor );
    else if(jetflavour ==4) FillHist("fh1dJetGenPts",GetPtCorrectedMC(jetgen), 1);  //this->fXsectionWeightingFactor );*/
}


void AliAnalysisTaskHFJetIPQA::FillIPTypePtHists(int jetflavour, double jetpt, int nTracks){
    //Fill histograms for jets which have largest, second largest and third largest impact parameter
    //tracks passing the selection criterion

    const char * subtype[5] = {"Unidentified","udsg","c","b","s"};

    for (Int_t iN = 1 ; iN <=3 ;++iN){
      if(nTracks<=iN) continue;
      FillHist(Form("fh1dJetRecPt_n_%i_%s_Accepted",iN,"all"),jetpt,1);     //*this->fXsectionWeightingFactor );

      if(jetflavour==0) continue;
      FillHist(Form("fh1dJetRecPt_n_%i_%s_Accepted",iN,subtype[jetflavour]),jetpt,1);     //*this->fXsectionWeightingFactor );
    }
      /*Double_t params [4] ={sImpParXY.at(ot).first,sImpParXYSig.at(ot).first,sImpParXYZ.at(ot).first,sImpParXYZSig.at(ot).first};
            Double_t weights[4] ={sImpParXY.at(ot).second,sImpParXYSig.at(ot).second,sImpParXYZ.at(ot).second,sImpParXYZSig.at(ot).second};
            Int_t    correctionwindex[4] ={sImpParXY.at(ot).trackLabel,sImpParXYSig.at(ot).trackLabel,sImpParXYZ.at(ot).trackLabel,sImpParXYZSig.at(ot).trackLabel};

            for (Int_t ost = 0 ; ost <4 ;++ost){
                TString hname = Form("%s%s",stype[ost],subord[ot]);
                if(fIsPythia)   FillHist(hname.Data(),jetpt,params[ost],weights[ost] *  1);     //this->fXsectionWeightingFactor );
                else  FillHist(hname.Data(),jetpt,params[ost], this->fXsectionWeightingFactor );


            }*/
            /*if(fIsPythia){

                            if(ot ==0){//N=1
                              FillHist("fh2dNMCWeightSpeciesPerJetPtN1_IP_all",correctionwindex[0]+0.5,jetpt, 1);     //this->fXsectionWeightingFactor );
                              FillHist("fh2dNMCWeightSpeciesPerJetPtN1_SIP_all",correctionwindex[1]+0.5,jetpt, 1);     //this->fXsectionWeightingFactor );
                                if(jetflavour==3) {//b
                                    FillHist("fh2dNMCWeightSpeciesPerJetPtN1_IP_b",correctionwindex[0]+0.5,jetpt, 1);     //this->fXsectionWeightingFactor );
                                    FillHist("fh2dNMCWeightSpeciesPerJetPtN1_SIP_b",correctionwindex[1]+0.5,jetpt, 1);     /this->fXsectionWeightingFactor );
                                }
                            else   if(jetflavour==2) {//c
                                FillHist("fh2dNMCWeightSpeciesPerJetPtN1_IP_c",correctionwindex[0]+0.5,jetpt, 1);     //this->fXsectionWeightingFactor );
                                FillHist("fh2dNMCWeightSpeciesPerJetPtN1_SIP_c",correctionwindex[1]+0.5,jetpt, 1);     //this->fXsectionWeightingFactor );
                            }
                            else   if(jetflavour==1) {//lf
                                FillHist("fh2dNMCWeightSpeciesPerJetPtN1_IP_lf",correctionwindex[0]+0.5,jetpt, 1);     //this->fXsectionWeightingFactor );
                                FillHist("fh2dNMCWeightSpeciesPerJetPtN1_SIP_lf",correctionwindex[1]+0.5,jetpt, 1);     //this->fXsectionWeightingFactor );
                            }

                        }
                            else  if(ot ==1){//N=2
                                FillHist("fh2dNMCWeightSpeciesPerJetPtN2_IP_all",correctionwindex[0]+0.5,jetpt, 1);     //this->fXsectionWeightingFactor );
                                FillHist("fh2dNMCWeightSpeciesPerJetPtN2_SIP_all",correctionwindex[1]+0.5,jetpt, 1);     //this->fXsectionWeightingFactor );
                                    if(jetflavour==3) {//b
                                        FillHist("fh2dNMCWeightSpeciesPerJetPtN2_IP_b",correctionwindex[0]+0.5,jetpt, 1);     //this->fXsectionWeightingFactor );
                                        FillHist("fh2dNMCWeightSpeciesPerJetPtN2_SIP_b",correctionwindex[1]+0.5,jetpt, 1);     //this->fXsectionWeightingFactor );
                                    }
                                    else   if(jetflavour==2) {//c
                                        FillHist("fh2dNMCWeightSpeciesPerJetPtN2_IP_c",correctionwindex[0]+0.5,jetpt, 1);     //this->fXsectionWeightingFactor );
                                        FillHist("fh2dNMCWeightSpeciesPerJetPtN2_SIP_c",correctionwindex[1]+0.5,jetpt, 1);     //this->fXsectionWeightingFactor );
                                    }
                                    else   if(jetflavour==1) {//lf
                                        FillHist("fh2dNMCWeightSpeciesPerJetPtN2_IP_lf",correctionwindex[0]+0.5,jetpt, 1);     //this->fXsectionWeightingFactor );
                                        FillHist("fh2dNMCWeightSpeciesPerJetPtN2_SIP_lf",correctionwindex[1]+0.5,jetpt, 1);     //this->fXsectionWeightingFactor );
                                    }

                                }
                            else  if(ot==2){//N=3
                                FillHist("fh2dNMCWeightSpeciesPerJetPtN3_IP_all",correctionwindex[0]+0.5,jetpt, 1);     //this->fXsectionWeightingFactor );
                                FillHist("fh2dNMCWeightSpeciesPerJetPtN3_SIP_all",correctionwindex[1]+0.5,jetpt, 1);     //this->fXsectionWeightingFactor );
                                    if(jetflavour==3) {//b
                                        FillHist("fh2dNMCWeightSpeciesPerJetPtN3_IP_b",correctionwindex[0]+0.5,jetpt, 1);     //this->fXsectionWeightingFactor );
                                        FillHist("fh2dNMCWeightSpeciesPerJetPtN3_SIP_b",correctionwindex[1]+0.5,jetpt, 1);     //this->fXsectionWeightingFactor );
                                    }
                                    else   if(jetflavour==2) {//c
                                        FillHist("fh2dNMCWeightSpeciesPerJetPtN3_IP_c",correctionwindex[0]+0.5,jetpt, 1);     //this->fXsectionWeightingFactor );
                                        FillHist("fh2dNMCWeightSpeciesPerJetPtN3_SIP_c",correctionwindex[1]+0.5,jetpt, 1);     //this->fXsectionWeightingFactor );
                                    }
                                    else   if(jetflavour==1) {//lf
                                        FillHist("fh2dNMCWeightSpeciesPerJetPtN3_IP_lf",correctionwindex[0]+0.5,jetpt, 1);     //this->fXsectionWeightingFactor );
                                        FillHist("fh2dNMCWeightSpeciesPerJetPtN3_SIP_lf",correctionwindex[1]+0.5,jetpt, 1);     //this->fXsectionWeightingFactor );
                                    }
                                }
                                for (Int_t ost = 0 ; ost <4 ;++ost){
                                    TString hname = Form("%s%s%s",stype[ost],subtype[jetflavour],subord[ot]);
                                    FillHist(hname.Data(),jetpt,params[ost],weights[ost]* 1);     //this->fXsectionWeightingFactor );
                                }
                            }*/
}

void AliAnalysisTaskHFJetIPQA::FillTrackTypeResHists(){
   printf("Filling track type resolution hists");

   /* if(GetImpactParameterWrtToJet((AliAODTrack*)trackV,(AliAODEvent*)InputEvent(),jetrec,dca,cov,xyzatcda,sign)){
      if(fEventVertex) {
        delete fEventVertex;
        fEventVertex =nullptr;
      }
      dca[0]=fabs(dca[0]);
      Double_t cursImParXYSig  =TMath::Abs(GetValImpactParameter(kXYSig,dca,cov))*sign;
      Double_t cursImParXYZSig =TMath::Abs(GetValImpactParameter(kXYZSig,dca,cov))*sign;

      Int_t corridx=-1;double ppt;
      (fIsPythia&&fDoMCCorrection) ? TrackWeight = GetMonteCarloCorrectionFactor(trackV,corridx,ppt) : TrackWeight =1;
                  Double_t cursImParXY     =TMath::Abs(GetValImpactParameter(   kXY,dca,cov))*sign;
                  Double_t cursImParXYZ    =TMath::Abs(GetValImpactParameter(   kXYZ,dca,cov))*sign;

                if(fIsPythia){
                  if(is_udgjet){
                      if (IsTrackAcceptedJP((AliAODTrack*)trackV,6)){
                          FillHist("fh2dJetSignedImpParXYSignificanceudg_light_resfunction_6ITShits",trackV->Pt(),cursImParXYSig,TrackWeight);     //this->fXsectionWeightingFactor );
                          //FillHist("fh2dJetSignedImpParXYZSignificanceudg_light_resfunction_6ITShits",trackV->Pt(),cursImParXYZSig,TrackWeight);     //this->fXsectionWeightingFactor );
                      }
                      else if (IsTrackAcceptedJP((AliAODTrack*)trackV,5)){
                          FillHist("fh2dJetSignedImpParXYSignificanceudg_light_resfunction_5ITShits",trackV->Pt(),cursImParXYSig,TrackWeight);     //this->fXsectionWeightingFactor );
                          //FillHist("fh2dJetSignedImpParXYZSignificanceudg_light_resfunction_5ITShits",trackV->Pt(),cursImParXYZSig,TrackWeight);     //this->fXsectionWeightingFactor );
                      }
                      else if (IsTrackAcceptedJP((AliAODTrack*)trackV,4)){
                          FillHist("fh2dJetSignedImpParXYSignificanceudg_light_resfunction_4ITShits",trackV->Pt(),cursImParXYSig,TrackWeight);     //this->fXsectionWeightingFactor );
                          //FillHist("fh2dJetSignedImpParXYZSignificanceudg_light_resfunction_4ITShits",trackV->Pt(),cursImParXYZSig,TrackWeight);     //this->fXsectionWeightingFactor );
                      }
                      else if (IsTrackAcceptedJP((AliAODTrack*)trackV,3)){
                          FillHist("fh2dJetSignedImpParXYSignificanceudg_light_resfunction_3ITShits",trackV->Pt(),cursImParXYSig,TrackWeight);     //this->fXsectionWeightingFactor );
                          //FillHist("fh2dJetSignedImpParXYZSignificanceudg_light_resfunction_3ITShits",trackV->Pt(),cursImParXYZSig,TrackWeight);     //this->fXsectionWeightingFactor );
                      }
                  }
                  if(IsFromElectron((AliAODTrack*)trackV)){
                      if(is_udgjet){
                          if (IsTrackAcceptedJP((AliAODTrack*)trackV,6)){
                              FillHist("fh2dJetSignedImpParXYSignificanceudg_light_resfunction_6ITShitsElectrons",trackV->Pt(),cursImParXYSig,TrackWeight);     //this->fXsectionWeightingFactor );
                              //FillHist("fh2dJetSignedImpParXYZSignificanceudg_light_resfunction_6ITShitsElectrons",trackV->Pt(),cursImParXYZSig,TrackWeight);     //this->fXsectionWeightingFactor );
                          }
                          else if (IsTrackAcceptedJP((AliAODTrack*)trackV,5)){
                              FillHist("fh2dJetSignedImpParXYSignificanceudg_light_resfunction_5ITShitsElectrons",trackV->Pt(),cursImParXYSig,TrackWeight);     //this->fXsectionWeightingFactor );
                              //FillHist("fh2dJetSignedImpParXYZSignificanceudg_light_resfunction_5ITShitsElectrons",trackV->Pt(),cursImParXYZSig,TrackWeight);     //this->fXsectionWeightingFactor );
                          }
                          else if (IsTrackAcceptedJP((AliAODTrack*)trackV,4)){
                              FillHist("fh2dJetSignedImpParXYSignificanceudg_light_resfunction_4ITShitsElectrons",trackV->Pt(),cursImParXYSig,TrackWeight);     //this->fXsectionWeightingFactor );
                              //FillHist("fh2dJetSignedImpParXYZSignificanceudg_light_resfunction_4ITShitsElectrons",trackV->Pt(),cursImParXYZSig,TrackWeight);     //this->fXsectionWeightingFactor );
                          }
                          else if (IsTrackAcceptedJP((AliAODTrack*)trackV,3)){
                              FillHist("fh2dJetSignedImpParXYSignificanceudg_light_resfunction_3ITShitsElectrons",trackV->Pt(),cursImParXYSig,TrackWeight);     //this->fXsectionWeightingFactor );
                              //FillHist("fh2dJetSignedImpParXYZSignificanceudg_light_resfunction_3ITShitsElectrons",trackV->Pt(),cursImParXYZSig,TrackWeight);     //this->fXsectionWeightingFactor );
                          }
                      }
                  }
                  else if(IsFromPion((AliAODTrack*)trackV)){
                      if(is_udgjet){
                          if (IsTrackAcceptedJP((AliAODTrack*)trackV,6)){
                              FillHist("fh2dJetSignedImpParXYSignificanceudg_light_resfunction_6ITShitsPions",trackV->Pt(),cursImParXYSig,TrackWeight);     //this->fXsectionWeightingFactor );
                              //FillHist("fh2dJetSignedImpParXYZSignificanceudg_light_resfunction_6ITShitsPions",trackV->Pt(),cursImParXYZSig,TrackWeight);     //this->fXsectionWeightingFactor );
                          }
                          else if (IsTrackAcceptedJP((AliAODTrack*)trackV,5)){
                              FillHist("fh2dJetSignedImpParXYSignificanceudg_light_resfunction_5ITShitsPions",trackV->Pt(),cursImParXYSig,TrackWeight);     //this->fXsectionWeightingFactor );
                              //FillHist("fh2dJetSignedImpParXYZSignificanceudg_light_resfunction_5ITShitsPions",trackV->Pt(),cursImParXYZSig,TrackWeight);     //this->fXsectionWeightingFactor );
                          }
                          else if (IsTrackAcceptedJP((AliAODTrack*)trackV,4)){
                              FillHist("fh2dJetSignedImpParXYSignificanceudg_light_resfunction_4ITShitsPions",trackV->Pt(),cursImParXYSig,TrackWeight);     //this->fXsectionWeightingFactor );
                              //FillHist("fh2dJetSignedImpParXYZSignificanceudg_light_resfunction_4ITShitsPions",trackV->Pt(),cursImParXYZSig,TrackWeight);     //this->fXsectionWeightingFactor );
                          }
                          else if (IsTrackAcceptedJP((AliAODTrack*)trackV,3)){
                              FillHist("fh2dJetSignedImpParXYSignificanceudg_light_resfunction_3ITShitsPions",trackV->Pt(),cursImParXYSig,TrackWeight);     //this->fXsectionWeightingFactor );
                              //FillHist("fh2dJetSignedImpParXYZSignificanceudg_light_resfunction_3ITShitsPions",trackV->Pt(),cursImParXYZSig,TrackWeight);     //this->fXsectionWeightingFactor );
                          }
                      }
                  }
                  else if(IsFromKaon((AliAODTrack*)trackV)){
                      if(is_udgjet){
                          if (IsTrackAcceptedJP((AliAODTrack*)trackV,6)){
                              FillHist("fh2dJetSignedImpParXYSignificanceudg_light_resfunction_6ITShitsKaons",trackV->Pt(),cursImParXYSig,TrackWeight);     //this->fXsectionWeightingFactor );
                              //FillHist("fh2dJetSignedImpParXYZSignificanceudg_light_resfunction_6ITShitsKaons",trackV->Pt(),cursImParXYZSig,TrackWeight);     //this->fXsectionWeightingFactor );
                          }
                          else if (IsTrackAcceptedJP((AliAODTrack*)trackV,5)){
                              FillHist("fh2dJetSignedImpParXYSignificanceudg_light_resfunction_5ITShitsKaons",trackV->Pt(),cursImParXYSig,TrackWeight);     //this->fXsectionWeightingFactor );
                              //FillHist("fh2dJetSignedImpParXYZSignificanceudg_light_resfunction_5ITShitsKaons",trackV->Pt(),cursImParXYZSig,TrackWeight);     //this->fXsectionWeightingFactor );
                          }
                          else if (IsTrackAcceptedJP((AliAODTrack*)trackV,4)){
                              FillHist("fh2dJetSignedImpParXYSignificanceudg_light_resfunction_4ITShitsKaons",trackV->Pt(),cursImParXYSig,TrackWeight);     //this->fXsectionWeightingFactor );
                              //FillHist("fh2dJetSignedImpParXYZSignificanceudg_light_resfunction_4ITShitsKaons",trackV->Pt(),cursImParXYZSig,TrackWeight);     //this->fXsectionWeightingFactor );
                          }
                          else if (IsTrackAcceptedJP((AliAODTrack*)trackV,3)){
                              FillHist("fh2dJetSignedImpParXYSignificanceudg_light_resfunction_3ITShitsKaons",trackV->Pt(),cursImParXYSig,TrackWeight);     //this->fXsectionWeightingFactor );
                              //FillHist("fh2dJetSignedImpParXYZSignificanceudg_light_resfunction_3ITShitsKaons",trackV->Pt(),cursImParXYZSig,TrackWeight);     //this->fXsectionWeightingFactor );
                          }
                      }
                  }
                  else if(IsFromProton((AliAODTrack*)trackV)){
                      if(is_udgjet){
                          if (IsTrackAcceptedJP((AliAODTrack*)trackV,6)){
                              FillHist("fh2dJetSignedImpParXYSignificanceudg_light_resfunction_6ITShitsProtons",trackV->Pt(),cursImParXYSig,TrackWeight);     //this->fXsectionWeightingFactor );
                              //FillHist("fh2dJetSignedImpParXYZSignificanceudg_light_resfunction_6ITShitsProtons",trackV->Pt(),cursImParXYZSig,TrackWeight);     //this->fXsectionWeightingFactor );
                          }
                          else if (IsTrackAcceptedJP((AliAODTrack*)trackV,5)){
                              FillHist("fh2dJetSignedImpParXYSignificanceudg_light_resfunction_5ITShitsProtons",trackV->Pt(),cursImParXYSig,TrackWeight);     //this->fXsectionWeightingFactor );
                              //FillHist("fh2dJetSignedImpParXYZSignificanceudg_light_resfunction_5ITShitsProtons",trackV->Pt(),cursImParXYZSig,TrackWeight);     //this->fXsectionWeightingFactor );
                          }
                          else if (IsTrackAcceptedJP((AliAODTrack*)trackV,4)){
                              FillHist("fh2dJetSignedImpParXYSignificanceudg_light_resfunction_4ITShitsProtons",trackV->Pt(),cursImParXYSig,TrackWeight);     //this->fXsectionWeightingFactor );
                              //FillHist("fh2dJetSignedImpParXYZSignificanceudg_light_resfunction_4ITShitsProtons",trackV->Pt(),cursImParXYZSig,TrackWeight);     //this->fXsectionWeightingFactor );
                          }
                          else if (IsTrackAcceptedJP((AliAODTrack*)trackV,3)){
                              FillHist("fh2dJetSignedImpParXYSignificanceudg_light_resfunction_3ITShitsProtons",trackV->Pt(),cursImParXYSig,TrackWeight);     //this->fXsectionWeightingFactor );
                              //FillHist("fh2dJetSignedImpParXYZSignificanceudg_light_resfunction_3ITShitsProtons",trackV->Pt(),cursImParXYZSig,TrackWeight);     //this->fXsectionWeightingFactor );
                          }
                      }
                  }
                      //Fill jet probability ipsig histograms for template fitting
                  const char * subtype_jp [4] = {"","udsg","c","b"};*/
}

double AliAnalysisTaskHFJetIPQA::DoUESubtraction(AliJetContainer* &jetcongen, AliJetContainer* &jetconrec, AliEmcalJet* &jetrec, double jetpt){
    //_________________________
    //Underlying Event Subtraction
    if((!(jetconrec->GetRhoParameter() == nullptr)))
    {
        printf("Correct for Underlying Event.\n");
        jetpt = jetpt - jetconrec->GetRhoVal() * jetrec->Area();
    }
    if(fIsPythia){
        if (jetrec->MatchedJet()) {
            Double_t genpt = jetrec->MatchedJet()->Pt();
            if((!(jetcongen->GetRhoParameter() == nullptr)))
            {
                printf("Correct for Underlying Event.\n");
                genpt = genpt - jetcongen->GetRhoVal() * jetrec->MatchedJet()->Area();
            }
            FillHist("fh2dJetGenPtVsJetRecPt",genpt,jetpt,1);    // this->fXsectionWeightingFactor );
        }
    }
    return jetpt;
}

Bool_t AliAnalysisTaskHFJetIPQA::IsEventAccepted(AliVEvent *ev){
    if(!fEventCuts.AcceptEvent(ev)){
        return kFALSE;
    }

    //if(!fMCRejectFilter) return true;
    if(!(fIsPythia)) return true; // Only relevant for pt-hard production
    AliDebugStream(1) << "Using custom MC outlier rejection" << std::endl;
    //auto partjets =GetJetContainer("mcparticles");
    AliJetContainer * partjets=static_cast<AliJetContainer*>(fJetCollArray.At(1));
    if(!partjets){
      printf("No particle container found\n");
      return true;
    }

      // Check whether there is at least one particle level jet with pt above n * event pt-hard
      auto jetiter = partjets->accepted();
      auto max = std::max_element(jetiter.begin(), jetiter.end(), [](const AliEmcalJet *lhs, const AliEmcalJet *rhs ) { return lhs->Pt() < rhs->Pt(); });
      if(max != jetiter.end()) {
        // At least one jet found with pt > n * pt-hard
        AliDebugStream(1) << "Found max jet with pt " << (*max)->Pt() << " GeV/c" << std::endl;
        if((*max)->Pt() > fAnalysisCuts[bAnalysisCut_PtHardAndJetPtFactor] * fPtHard){
            //printf("Refuse jet with jetpt=%f, fPtHard=%f, fPtHardAndJetPtFactor=%f\n",(*max)->Pt(), fPtHard,fAnalysisCuts[bAnalysisCut_PtHardAndJetPtFactor]);
            return false;
        }
      }
     return true;
}

Bool_t AliAnalysisTaskHFJetIPQA::Run(){


    //*******************************
    //Selection
    FillGeneralHistograms();
    /*Vertex Pos Selection*/
    fEventVertex = dynamic_cast<const AliAODVertex*>(InputEvent()->GetPrimaryVertex());
    fIsSameEvent_n1 = kFALSE;
    fIsSameEvent_n2 = kFALSE;
    fIsSameEvent_n3 = kFALSE;

    AliVEvent *ev = InputEvent();
    IsEventAccepted(ev);
    IncHist("fh1dEventsAcceptedInRun",1);

    Bool_t HasImpactParameter = kFALSE;
    Double_t dca[2] = {-99999,-99999};
    Double_t cov[3] = {-99999,-99999,-99999};
    Double_t TrackWeight       = 1;
    AliVTrack* trackV = NULL;
    fIsEsd =  (InputEvent()->IsA()==AliESDEvent::Class())? kTRUE : kFALSE;
   // EventwiseCleanup();
    if(fIsPythia){
        if(fIsEsd){
            fMCEvent = dynamic_cast<AliMCEvent*>(MCEvent()) ;
            if (!fMCEvent){
                AliError("Could not retrieve  MC particles! Returning");
                return kFALSE;
            }
        }
        else{
            fMCArray = static_cast<TClonesArray*>(InputEvent()->FindListObject(AliAODMCParticle::StdBranchName()));
            if (!fMCArray){
                AliError("Could not retrieve AOD MC particles! Returning");
                return kFALSE;
            }
        }
    }
    //if(fIsPythia)FillParticleCompositionEvent(); //Added  for in cone vs inclusive comparison

    FillHist("fh1dNoParticlesPerEvent",InputEvent()->GetNumberOfTracks(),1);

    //************************
    //Investigate Track Corrections: Smearing & particle weighting
    for(long itrack= 0; itrack<InputEvent()->GetNumberOfTracks();++itrack)
    {
        trackV = static_cast<AliVTrack*>(InputEvent()->GetTrack(itrack));
        if(!trackV) {
            AliInfo("Could not retrieve Track");
            continue;
        }
        IncHist("fh1dTracksAccepeted",1);
        if(!IsTrackAccepted(trackV,-1)) {
            IncHist("fh1dTracksAccepeted",3);
            continue;
        }
        if(fRunSmearing)SmearTrack((AliAODTrack*)trackV);
        IncHist("fh1dTracksAccepeted",2);
        FillHist("fh2dAcceptedTracksEtaPhi",trackV->Eta(),trackV->Phi(),1); //this->fXsectionWeightingFactor );
        TrackWeight =1;dca[0]=-9999;dca[1]=-9999;cov[0]=-9999;cov[1]=-9999;cov[2]=-9999;
        HasImpactParameter =kFALSE;
        Double_t xyzatdca[3];
        if (GetImpactParameter(((AliAODTrack*)trackV),(AliAODEvent *)InputEvent(), dca, cov,xyzatdca))HasImpactParameter =kTRUE;
        if(fEventVertex) {
            delete fEventVertex;
            fEventVertex =nullptr;
        }
        if(!HasImpactParameter)  continue;
        /*if(fIsPythia){
            Int_t corrpartidx =-1;
            double ppt;
            //if(fDoMCCorrection) TrackWeight *= GetMonteCarloCorrectionFactor(trackV,corrpartidx,ppt);
        }*/
        FillTrackHistograms(trackV,dca,cov,TrackWeight);
    }

    //**********************************
    //JetMatching between generated and reconstructed Jets
    AliJetContainer *  jetconrec = static_cast<AliJetContainer*>(fJetCollArray.At(0));
    //printf("In Program %f < jetpt <%f, %f < jeteta < %f\n",fAnalysisCuts[bAnalysisCut_MinJetPt],fAnalysisCuts[bAnalysisCut_MaxJetPt],fAnalysisCuts[bAnalysisCut_MinJetEta],fAnalysisCuts[bAnalysisCut_MaxJetEta] );
    FillHist("fh1dNoJetsPerEvent",jetconrec->GetNJets(),1);
    if (!jetconrec) return kFALSE;
    AliJetContainer * jetcongen = nullptr;
    AliEmcalJet * jetgen  = nullptr;

    if(fIsPythia){
        jetcongen = static_cast<AliJetContainer*>(fJetCollArray.At(1));
        if(!MatchJetsGeometricDefault()) AliInfo("Jet matching did not succeed!");
        jetcongen->ResetCurrentID();
        while ((jetgen = jetcongen->GetNextAcceptJet()))
        {
            if (!jetgen) continue;
            //Int_t jetflavour =0;
            //Bool_t is_udgjet = kFALSE;
            //jetflavour =IsMCJetPartonFast(jetgen,fJetRadius,is_udgjet);
            //FillGenHistograms(jetflavour, jetgen);
        }
        jetcongen->ResetCurrentID();
        jetconrec->ResetCurrentID();
    }

    //*************************************************
    // Loop over reconstructed/matched jets for template creation and analysis
    AliEmcalJet * jetrec  = nullptr;
    AliEmcalJet * jetmatched  = nullptr;
    jetconrec->ResetCurrentID();
    Double_t jetpt=0;
    while ((jetrec = jetconrec->GetNextAcceptJet()))
    {//start jetloop
        if(!jetrec) continue;
        jetpt = jetrec->Pt();
        if(fDoUnderlyingEventSub)jetpt=DoUESubtraction(jetcongen, jetconrec,jetrec, jetpt);

        FillHist("fh1dJetArea",jetrec->Area(),1);

        //________________________
        //Determination of Jet Flavour
        Int_t jetflavour=0;
        Bool_t is_udgjet = kFALSE;
        if(fIsPythia){
          jetmatched = nullptr;
          jetmatched =jetrec->MatchedJet();
          if(jetmatched){
            jetflavour = IsMCJetPartonFast(jetmatched,fJetRadius,is_udgjet); //Event based association to save memory
          }
          else{
            jetflavour=0;
          }
        }
        FillRecHistograms( jetflavour, jetpt, jetrec->Eta(),jetrec->Phi());
        if(fDoLundPlane)RecursiveParents(jetrec, jetconrec);

        //_____________________________
        //Determination of impact parameters
        std::vector<SJetIpPati> sImpParXY,sImpParXYZ,sImpParXYSig,sImpParXYZSig;
        AliVParticle *vp=0x0;
        int NJetParticles=0;  //Used for counting particles per jet
        Double_t dca[2] = {-99999,-99999};
        Double_t cov[3] = {-99999,-99999,-99999};
        Double_t sign=0;

        for(UInt_t i = 0; i < jetrec->GetNumberOfTracks(); i++) {//start trackloop
          TrackWeight=1;
          Double_t xyzatcda[3];

          vp = static_cast<AliVParticle*>(jetrec->TrackAt(i, jetconrec->GetParticleContainer()->GetArray()));
          if (!vp){
            AliError("AliVParticle associated to constituent not found");
            continue;
          }
          AliVTrack *vtrack = dynamic_cast<AliVTrack*>(vp);
          if (!vtrack) {
            AliError(Form("Could not receive track%d\n", i));
            continue;
          }
          AliAODTrack *trackV = dynamic_cast<AliAODTrack*>(vtrack);

          if (!trackV || !jetrec)            continue;
          if (fIsPythia&&!IsTrackAccepted((AliAODTrack*)trackV,jetflavour))   continue;
          ++NJetParticles;

          //FillTrackTypeResHists();

          if(GetImpactParameterWrtToJet((AliAODTrack*)trackV,(AliAODEvent*)InputEvent(),jetrec,dca,cov,xyzatcda,sign, jetflavour)){
            if(fEventVertex) {
              delete fEventVertex;
              fEventVertex =nullptr;
            }
            Int_t corridx=-1;double ppt;
            //(fIsPythia&&fDoMCCorrection) ? TrackWeight = GetMonteCarloCorrectionFactor(trackV,corridx,ppt) : TrackWeight =1;
            dca[0]=fabs(dca[0]);
            Double_t cursImParXY     =TMath::Abs(GetValImpactParameter(   kXY,dca,cov))*sign;
            Double_t cursImParXYSig  =TMath::Abs(GetValImpactParameter(kXYSig,dca,cov))*sign;
            Double_t cursImParXYZ    =TMath::Abs(GetValImpactParameter(   kXYZ,dca,cov))*sign;
            Double_t cursImParXYZSig =TMath::Abs(GetValImpactParameter(kXYZSig,dca,cov))*sign;
            FillHist("fh2dJetSignedImpParXY"            ,jetpt,cursImParXY,TrackWeight);     //*this->fXsectionWeightingFactor );
            FillHist("fh2dJetSignedImpParXYSignificance",jetpt,cursImParXYSig,TrackWeight);     //*this->fXsectionWeightingFactor );

            const char * subtype [5] = {"Unidentified","udsg","c","b","s"};
            if(fIsPythia){
              FillHist(Form("fh2dJetSignedImpParXY%s",subtype[jetflavour]),jetpt,cursImParXY,TrackWeight);     //*this->fXsectionWeightingFactor );
              FillHist(Form("fh2dJetSignedImpParXYSignificance%s",subtype[jetflavour]),jetpt,cursImParXYSig,TrackWeight);     //*this->fXsectionWeightingFactor );
            }
            SJetIpPati a(cursImParXY, TrackWeight,kFALSE,kFALSE,corridx,trackV->Pt()); sImpParXY.push_back(a);
            SJetIpPati b(cursImParXYZ, TrackWeight,kFALSE,kFALSE,corridx,trackV->Pt()); sImpParXYZ.push_back(b);
            SJetIpPati c(cursImParXYSig, TrackWeight,kFALSE,kFALSE,corridx,trackV->Pt());sImpParXYSig.push_back(c);
            SJetIpPati d(cursImParXYZSig, TrackWeight,kFALSE,kFALSE,corridx,trackV->Pt());sImpParXYZSig.push_back(d);
            //printf("curImParXY=%f, TrackWeight=%f,  corridx=%i, pt=%f\n",cursImParXYSig,TrackWeight,corridx, trackV->Pt());

           }
         }//end trackloop

                FillHist("fh1dParticlesPerJet",NJetParticles,1);
                //_________________________
                //Sorting of Impact Parameters
                bool hasIPs[3] ={kFALSE,kFALSE,kFALSE};
                Double_t ipval[3] = {-9999.,-9999.,-9999.};

                std::sort(sImpParXY.begin(),sImpParXY.end(),        AliAnalysisTaskHFJetIPQA::mysort);
                std::sort(sImpParXYSig.begin(),sImpParXYSig.end(),  AliAnalysisTaskHFJetIPQA::mysort);
                std::sort(sImpParXYZ.begin(),sImpParXYZ.end(),      AliAnalysisTaskHFJetIPQA::mysort);
                std::sort(sImpParXYZSig.begin(),sImpParXYZSig.end(),AliAnalysisTaskHFJetIPQA::mysort);

                if((int)sImpParXYSig.size()>0) hasIPs[0]=kTRUE;
                if((int)sImpParXYSig.size()>1) hasIPs[1]=kTRUE;
                if((int)sImpParXYSig.size()>2) hasIPs[2]=kTRUE;

                if(hasIPs[0]){
                    ipval[0] =sImpParXYSig.at(0).first;
                    //printf("HasIP0, ipval[0]=%f\n", ipval[0]);
                  }
                if(hasIPs[1]){
                    ipval[1] =sImpParXYSig.at(1).first;
                    //printf("HasIP1, ipval[1]=%f\n",ipval[1]);
                  }
                if(hasIPs[2]){
                    ipval[2] =sImpParXYSig.at(2).first;
                    //printf("HasIP2, ipval[2]=%f\n", ipval[2]);
                  }
                //if(hasIPs[0])printf("N=1: cursImParXY=%f, TrackWeight=%f,corridx=%i, pt=%f\n",sImpParXYSig.at(0).first, sImpParXYSig.at(0).second, sImpParXYSig.at(0).trackLabel, sImpParXYSig.at(0).trackpt);
                //if(hasIPs[1])printf("N=2: cursImParXY=%f, TrackWeight=%f, corridx=%i, pt=%f\n",sImpParXYSig.at(1).first, sImpParXYSig.at(1).second, sImpParXYSig.at(1).trackLabel, sImpParXYSig.at(1).trackpt);
                //if(hasIPs[2])printf("N=3: cursImParXY=%f, TrackWeight=%f, corridx=%i, pt=%f\n",sImpParXYSig.at(2).first, sImpParXYSig.at(2).second, sImpParXYSig.at(2).trackLabel, sImpParXYSig.at(2).trackpt);
                //printf("*********************************************************\n");

                if(hasIPs[0])FillHist("fh1dTrackPt_n_1_all_Accepted",sImpParXYSig.at(0).trackpt,1);
                if(hasIPs[1])FillHist("fh1dTrackPt_n_2_all_Accepted",sImpParXYSig.at(1).trackpt,1);
                if(hasIPs[2])FillHist("fh1dTrackPt_n_3_all_Accepted",sImpParXYSig.at(2).trackpt,1);

                /*if(fFillCorrelations || fUseTreeForCorrelations){
                    FillCorrelations(hasIPs,ipval,jetpt);
                    if(fFillCorrelations && ! fUseTreeForCorrelations){
                        if(!fIsMixSignalReady_n1 && hasIPs[0]) SetMixDCA(1,ipval[0] );
                        if(!fIsMixSignalReady_n2 && hasIPs[1]) SetMixDCA(2,ipval[1] );
                        if(!fIsMixSignalReady_n3 && hasIPs[2]) SetMixDCA(3,ipval[2] );
                    }
                }*/

                //________________________________
                //MC Track Counting
                if(fIsPythia&&fDoTCTagging){
                  bool ** kTagDec=new bool*[fNThresholds];
                  for(int iThresh=0;iThresh<fNThresholds;iThresh++){
                    kTagDec[iThresh]=new bool[6];
                    for(int iType=0;iType<6;iType++){
                      kTagDec[iThresh][iType]=0;
                    }
                  }
                  DoJetTaggingThreshold(jetpt, hasIPs,ipval, kTagDec);
                  FillTCEfficiencyHists(kTagDec, jetflavour, jetpt,hasIPs[0]);
                }

                //___________________________________
                //MC Probability Tagging
                if(fIsPythia&&fDoTCTagging){
                  double probval=0;
                  probval=GetTrackProbability(jetpt,hasIPs, ipval);
                  if(probval>0)FillProbabilityHists(jetpt,  probval, jetflavour);
                }

                if(sImpParXY.size()!=0){
                  FillHist("fh2dNoAcceptedTracksvsJetArea",(int)sImpParXY.size(),jetrec->Area(),1);
                }
                sImpParXY.clear();
                sImpParXYSig.clear();
                sImpParXYZ.clear();
                sImpParXYZSig.clear();
              }//end jetloop*/
            return kTRUE;
          }



                        Double_t AliAnalysisTaskHFJetIPQA::GetLocalAlphaAOD(AliAODTrack * track)
                        {
                            AliExternalTrackParam etp; etp.CopyFromVTrack(track);
                            return etp.GetAlpha();
                        }
                        Double_t AliAnalysisTaskHFJetIPQA::GetLocalThetaAOD(AliAODTrack * track)
                        {
    // convert to AliExternalTrackParam
                            AliExternalTrackParam etp; etp.CopyFromVTrack(track);
    // propagate

                            Double_t dv[2],dcov[3];
                            const Double_t kBeampiperadius=3;
                            AliVEvent * eev = (AliVEvent*)InputEvent();
                            const  AliVVertex *vtxESDSkip =(const  AliVVertex *) (InputEvent()->GetPrimaryVertex())  ;
                            if(!vtxESDSkip) return -9999;
                            if(!(etp.PropagateToDCA(vtxESDSkip, eev->GetMagneticField(), kBeampiperadius, dv, dcov)))return -9999.;
    // update track position and momentum
                            return etp.Theta();
                        }

                        Double_t AliAnalysisTaskHFJetIPQA::GetTrackCurvature(AliAODTrack * track)
                        {
    // convert to AliExternalTrackParam
                            AliVEvent * eev = (AliVEvent*)InputEvent();

                            AliExternalTrackParam etp; etp.CopyFromVTrack(track);

                            return etp.GetC(eev->GetMagneticField());
                        }


/*! \brief IsSelected
 *
 *
 * Enable event selection
 */
                        Bool_t AliAnalysisTaskHFJetIPQA::IsVertexSelected(const AliVVertex *vertex){
                            if(!vertex) return kFALSE;
    //Printf("GetNContributors %i",((AliAODVertex*)vertex)->GetNContributors());

                            if(((AliAODVertex*)vertex)->GetNContributors()<1) {
                                return kFALSE;
                            }

    ///Printf("GetNContributors %i",((AliAODVertex*)vertex)->GetNContributors());
                            if(((AliAODVertex*)vertex)->GetNContributors()<(int)(fAnalysisCuts[bAnalysisCut_NContibutors])) {
                                return kFALSE;
                            }
                            if(TMath::Abs(((AliAODVertex*)vertex)->GetZ())>=fAnalysisCuts[bAnalysisCut_MaxVtxZ]) {
                                return kFALSE;
                            }
                            if((TMath::Abs(((AliAODVertex*)vertex)->GetX() - ((AliAODEvent*)InputEvent())->GetDiamondX()) > fAnalysisCuts[bAnalysisCut_SigmaDiamond]*sqrt(((AliAODEvent*)InputEvent())->GetSigma2DiamondX()))) {
                                return kFALSE;
                            }
                            if((TMath::Abs(((AliAODVertex*)vertex)->GetY() - ((AliAODEvent*)InputEvent())->GetDiamondY()) > fAnalysisCuts[bAnalysisCut_SigmaDiamond]*sqrt(((AliAODEvent*)InputEvent())->GetSigma2DiamondY()))) {
                                return kFALSE;
                            }
                            if((TMath::Abs(((AliAODVertex*)vertex)->GetZ() - ((AliAODEvent*)InputEvent())->GetDiamondZ()) > fAnalysisCuts[bAnalysisCut_SigmaDiamond]*sqrt(((AliAODEvent*)InputEvent())->GetSigma2DiamondZ()))) {
                                return kFALSE;
                            }
                            return kTRUE;
                        }


                        Bool_t AliAnalysisTaskHFJetIPQA::IsSelected(AliVEvent *event, Int_t &WhyRejected,ULong_t &RejectionBits){
                            WhyRejected =0;
                            Bool_t accept=kTRUE;
                            RejectionBits=000;
                            Bool_t isSelected = kFALSE;
                            isSelected =  (((AliInputEventHandler*)(AliAnalysisManager::GetAnalysisManager()->GetInputEventHandler()))->IsEventSelected() & AliVEvent::kAny);

                            if(!isSelected) {
                                WhyRejected=kPhysicsSelection;
                                return kFALSE;
                            }
                            if(!fEventVertex) {
                                WhyRejected=kNoVertex;
                                return kFALSE;
                            }
                            if(fEventVertex->GetNContributors()<1) {
                                WhyRejected=kNoContributors;
                                return kFALSE;
                            }

                            if(fEventVertex->GetChi2perNDF()>fAnalysisCuts[bAnalysisCut_Z_Chi2perNDF]) {
                                WhyRejected=kVertexChi2NDF;
                                return kFALSE;
                            }
                            if(fEventVertex->GetNContributors()<(int)(fAnalysisCuts[bAnalysisCut_NContibutors])) {
                                WhyRejected=kTooFewVtxContrib;
                                return kFALSE;
                            }

                            if(TMath::Abs(fEventVertex->GetZ())>=fAnalysisCuts[bAnalysisCut_MaxVtxZ]) {
                                WhyRejected=kZVtxOutFid;
                                return kFALSE;
                            }
                            return accept;
                        }
/*! \brief SetUseMonteCarloWeighingLinus
 *
 *
 * Setter for MC composition correction factors
 */
void AliAnalysisTaskHFJetIPQA::SetUseMonteCarloWeighingLinus(TH1F *Pi0, TH1F *Eta, TH1F *EtaP, TH1F *Rho, TH1F *Phi, TH1F *Omega, TH1F *K0s, TH1F *Lambda, TH1F *ChargedPi, TH1F *ChargedKaon, TH1F *Proton, TH1F *D0, TH1F *DPlus, TH1F *DStarPlus, TH1F *DSPlus, TH1F *LambdaC, TH1F *BPlus, TH1F *B0, TH1F *LambdaB, TH1F *BStarPlus)
  {
  for(Int_t i =1 ; i< Pi0->GetNbinsX()+1;++i){
  fBackgroundFactorLinus[bIdxPi0][i-1] =Pi0->GetBinContent(i);
  fBackgroundFactorLinus[bIdxEta][i-1] =Eta->GetBinContent(i);
  fBackgroundFactorLinus[bIdxEtaPrime][i-1] =EtaP->GetBinContent(i);
  fBackgroundFactorLinus[bIdxRho][i-1] =Rho->GetBinContent(i);
  fBackgroundFactorLinus[bIdxPhi][i-1] =Phi->GetBinContent(i);
  fBackgroundFactorLinus[bIdxOmega][i-1] =Omega->GetBinContent(i);
  fBackgroundFactorLinus[bIdxK0s][i-1] =K0s->GetBinContent(i);
  fBackgroundFactorLinus[bIdxLambda][i-1] =Lambda->GetBinContent(i);
  fBackgroundFactorLinus[bIdxPi][i-1] =ChargedPi->GetBinContent(i);
  fBackgroundFactorLinus[bIdxKaon][i-1] =ChargedKaon->GetBinContent(i);
  fBackgroundFactorLinus[bIdxProton][i-1] =Proton->GetBinContent(i);
  fBackgroundFactorLinus[bIdxD0][i-1] =D0->GetBinContent(i);
  fBackgroundFactorLinus[bIdxDPlus][i-1] =DPlus->GetBinContent(i);
  fBackgroundFactorLinus[bIdxDStarPlus][i-1] =DStarPlus->GetBinContent(i);
  fBackgroundFactorLinus[bIdxDSPlus][i-1] =DSPlus->GetBinContent(i);
  fBackgroundFactorLinus[bIdxLambdaC][i-1] =LambdaC->GetBinContent(i);
  fBackgroundFactorLinus[bIdxBPlus][i-1] =BPlus->GetBinContent(i);
  fBackgroundFactorLinus[bIdxB0][i-1] =B0->GetBinContent(i);
  fBackgroundFactorLinus[bIdxLambdaB][i-1] =LambdaB->GetBinContent(i);
  fBackgroundFactorLinus[bIdxBStarPlus][i-1] =BStarPlus->GetBinContent(i);
  }
  return;
}

void AliAnalysisTaskHFJetIPQA::SetFlukaFactor(TGraph* GraphOmega, TGraph* GraphXi, TGraph* K0Star, TGraph* Phi)
  {
   fGraphOmega=(TGraph*)GraphOmega;
   fGraphXi=(TGraph*)GraphXi;
   fK0Star=(TGraph*)K0Star;
   fPhi=(TGraph*)Phi;

   return;
  }



/*! \brief SetResFunction
 *
 *
 * Setter for resolution function (currently unused)
 */

                        Bool_t AliAnalysisTaskHFJetIPQA::SetResFunctionPID(const char * filename){
                            TFile  * jetProbfile=TFile::Open(filename,"READ");

                            int bins_low[5]  = {0,1,2,4,6};
                            int bins_high[5]  = {1,2,4,6,255};
                            int its_hits[4]  = {6,5,4,3};

                            const char * type[5] ={"Electron","Pion","Kaon","Proton",""};
                            for (int k=0;k<5;++k){
                                for (int i=0;i<4;++i){
                                    for (int j=0;j<5;++j){
                                        TGraph *fResulFkt = 0x0;
                                        if(k==4) fResulFkt = (TGraph*)jetProbfile->Get(Form("fResulFkt_ITS_%i_PT_%i_to_%i",its_hits[i],bins_low[j],bins_high[j]));
                                        else fResulFkt = (TGraph*)jetProbfile->Get(Form("fResulFkt_ITS_%i_PT_%i_to_%i_%s",its_hits[i],bins_low[j],bins_high[j],type[k]));
                                        if(!fResulFkt){
                                            return kFALSE;
                                        }
                                        else {
                                            fResolutionFunction [20*k + 4*j +i] = *fResulFkt;
                                            Printf("Added %i %i %i -> [%i][%i][%i]->[%i]",j,k,i,j,i,k,20*k + 4*j +i  );
                                            delete fResulFkt;
                                        }
                                    }
                                }
                            }
                            if(jetProbfile)jetProbfile->Close();

                            fUsePIDJetProb =kTRUE;
                            return kTRUE;

                        }

                        void AliAnalysisTaskHFJetIPQA::FillCorrelations(bool bn[3],double v[3], double jetpt ){
    //Fill all possible same jet distributions
                             fTREE_n1 = -999;
                             fTREE_n2 = -999;
                             fTREE_n3 = -999;
                             fTREE_pt = -999;
                                     
                             if(fUseTreeForCorrelations){
                                if(bn[0] && bn[1]){
                                   fTREE_n1 = v[0];
                                   fTREE_n2 = v[1];
                                   fTREE_n3 = v[2];
                                   fTREE_pt = jetpt;
                                   fCorrelationCrossCheck->Fill();                                   
                                }
                             }   
                            else if (fFillCorrelations && !fUseTreeForCorrelations )
                            {
                            if (bn[0] && bn[1]) {
                                FillHist("fh2dInclusiveCorrelationN1N2",v[0],v[1]);
                                if(jetpt>10 && jetpt <20)    FillHist("fh2dGreater10_20GeVCorrelationN1N2",v[0],v[1]);
                                if(jetpt>20 && jetpt <30)    FillHist("fh2dGreater20_30GeVCorrelationN1N2",v[0],v[1]);
                                if(jetpt>30 && jetpt <100)    FillHist("fh2dGreater30_100GeVCorrelationN1N2",v[0],v[1]);

                            }
                            if (bn[0] && bn[2]) {
                                FillHist("fh2dInclusiveCorrelationN1N3",v[1],v[2]);
                                if(jetpt>10 && jetpt <20)    FillHist("fh2dGreater10_20GeVCorrelationN1N3",v[0],v[2]);
                                if(jetpt>20 && jetpt <30)    FillHist("fh2dGreater20_30GeVCorrelationN1N3",v[0],v[2]);
                                if(jetpt>30 && jetpt <100)    FillHist("fh2dGreater30_100GeVCorrelationN1N3",v[0],v[2]);
                                FillHist("fh2dInclusiveCorrelationN2N3",v[0],v[2]);
                                if(jetpt>10 && jetpt <20)    FillHist("fh2dGreater10_20GeVCorrelationN2N3",v[1],v[2]);
                                if(jetpt>20 && jetpt <30)    FillHist("fh2dGreater20_30GeVCorrelationN2N3",v[1],v[2]);
                                if(jetpt>30 && jetpt <100)    FillHist("fh2dGreater30_100GeVCorrelationN2N3",v[1],v[2]);
                            }
    //Fill if possible mix distributions
                            bool n3wasReady = false;
                            double storedn3=-999;
                            if(bn[0]){

                                if(fIsMixSignalReady_n2) {
                                    double n2 =0;
                                    if(GetMixDCA(2,n2)) {
                                        FillHist("fh2dInclusiveCorrelationN1N2mix",v[0],n2);
                                        if(jetpt>10 && jetpt <20)    FillHist("fh2dGreater10_20GeVCorrelationN1N2mix",v[0],n2);
                                        if(jetpt>20 && jetpt <30)    FillHist("fh2dGreater20_30GeVCorrelationN1N2mix",v[0],n2);
                                        if(jetpt>30 && jetpt <100)    FillHist("fh2dGreater30_100GeVCorrelationN1N2mix",v[0],n2);
                                    }
                                }
                                if(fIsMixSignalReady_n3) {
                                    double n3 =0;
                                    if(GetMixDCA(3,n3)) {
                                        n3wasReady=true;
                                        storedn3=n3;
                                        FillHist("fh2dInclusiveCorrelationN1N3mix",v[0],n3);
                                        if(jetpt>10 && jetpt <20)    FillHist("fh2dGreater10_20GeVCorrelationN1N3mix",v[0],n3);
                                        if(jetpt>20 && jetpt <30)    FillHist("fh2dGreater20_30GeVCorrelationN1N3mix",v[0],n3);
                                        if(jetpt>30 && jetpt <100)    FillHist("fh2dGreater30_100GeVCorrelationN1N3mix",v[0],n3);
                                    }
                                }
                            }

                            if(bn[1]) {
                                if(n3wasReady) {
                                    double n3 =0;
                                    n3=storedn3;
                                    FillHist("fh2dInclusiveCorrelationN2N3mix",v[1],n3);
                                    if(jetpt>10 && jetpt <20)    FillHist("fh2dGreater10_20GeVCorrelationN2N3mix",v[1],n3);
                                    if(jetpt>20 && jetpt <30)    FillHist("fh2dGreater20_30GeVCorrelationN2N3mix",v[1],n3);
                                    if(jetpt>30 && jetpt <100)    FillHist("fh2dGreater30_100GeVCorrelationN2N3mix",v[1],n3);
                                }
                            }
                            }
                            return;
                        }



void AliAnalysisTaskHFJetIPQA::UserCreateOutputObjects(){
  Printf("Analysing Jets with Radius: R=%f\n",fJetRadius);

  TString BJetCuts[25] = {
    "#sigma_{Dia}",  //0
    "#sigma_{z}",       //1
    "#sigma_{y}",       //2
    "RelError_{z}",     //3
    "RelError_{y}",     //4
    "N_{Cont}",       //5
    "MaxVtxZ",        //6
    "d_{JetTrack}",   //7
    "d_{z}",          //8
    "d_{xy}",         //9
    "DecayLength",    //10
    "d_{chi2/ndf}", //11
    "p_{T,Track}^{min}",      //12
    "p_{T,TrackMC}^{min}",    //13
    "MinTPCClus",     //14
    "n_{ITS Hits}",   //15
    "chi2_{track}", //16
    "p_{T,Jet}^{min}",//17
    "p_{T,Jet}^{max}",//18
    "#eta_{Jet}^{min}",       //19
    "#eta_{Jet}^{max}",        //20
    "SPD Hits", //21
    "Kink",//22
    "TPC Refit",//23
    "ITS Refit" //24
  };

  fIsMixSignalReady_n1=kFALSE;
  fIsMixSignalReady_n2=kFALSE;
  fIsMixSignalReady_n3=kFALSE;
  fn1_mix =-1;
  fn2_mix =-1;
  fn3_mix =-1;
  fIsSameEvent_n1=kFALSE;
  fIsSameEvent_n2=kFALSE;
  fIsSameEvent_n3=kFALSE;

  AliAnalysisTaskEmcal::UserCreateOutputObjects();
  AliAnalysisManager *mgr = AliAnalysisManager::GetAnalysisManager();
  if(!mgr) AliError("Analysis manager not found!");
  AliVEventHandler *evhand = mgr->GetInputEventHandler();
  if (!evhand) AliError("Event handler not found!");
  if (evhand->InheritsFrom("AliESDInputHandler"))  fIsEsd = kTRUE;
  else fIsEsd = kFALSE;
  OpenFile(1);

  Double_t lowIPxy =-1.;  //ranges of xy axis of fh2dTracksImpParXY and fh2dTracksImpParZ
  Double_t highIPxy =1.;

  OpenFile(1);
  if(fIsPythia){
    if(!fPidResponse)   fPidResponse = ((AliInputEventHandler*)(AliAnalysisManager::GetAnalysisManager()->GetInputEventHandler()))->GetPIDResponse();
    if (!fPidResponse) {
      AliFatal("NULL PID response");
    }
    if(!fCombined) fCombined = new AliPIDCombined();
  }/*
    //Graphs currently not in use
                                               const Int_t gfProtonN = 9;
                                                const Int_t gfAntiProtonN = 18;
                                                const Int_t gfAntiLambdaN = 34;
                                                const Int_t gfLambdaN = 2;
                                                const Int_t gfKMinusN =13 ;
                                                Double_t gfProtonX [gfProtonN] ={0,0.534483,1.29741,2.21552,3.0819,3.92241,4.5819,5.39655,1000};
                                                Double_t gfProtonY [gfProtonN] ={0.990964,0.990964,0.990964,0.990964,0.990964,0.990964,0.990964,0.990964,0.990964};
                                                Double_t gfAntiProtonX [gfAntiProtonN]  = {0,0.806034,0.922414,1.09052,1.28448,1.5431,1.73707,1.89224,2.17672,2.43534,2.74569,3.06897,
                                                   3.52155,3.88362,4.38793,5.03448,5.38362, 1000};
                                                   Double_t gfAntiProtonY [gfAntiProtonN]  = {0.922892,0.922892,	0.930723,	0.939157,0.94397,0.95241,0.956627,0.959639,0.964458,
                                                       0.966867,0.971084,0.974096,0.978313,0.98012,0.983735,0.986747,0.989157,0.989157};
                                                       Double_t gfAntiLambdaX [gfAntiLambdaN]  = {0.,0.55555,0.64646,0.75757,	0.84848,0.94949,1.06061,1.15152,1.24242,1.35354,1.44444,
                                                           1.54545,1.66667,1.75758,1.84848,1.9596,2.09091,2.30303,2.50505,2.68687,2.90909,3.11111,
                                                           3.31313,3.51515,3.69697,3.89899,4.20202,4.66667,5.21212,5.74747,6.50505,7.51515,9.0101,1000};
                                                           Double_t gfAntiLambdaY [gfAntiLambdaN]  = {0.864925,0.864925,0.895896,0.908209,0.915672,0.921269,0.926866,0.931343,0.935821,0.938806,0.942164,
                                                               0.945149,0.947761,0.95,0.952612,0.954478,0.957836,0.960821,0.96306,0.965672,0.968657,0.970149,
                                                               0.972015,0.973507,0.975,0.976493,0.978358,0.981343,0.983955,0.986194,0.988433,0.991045,0.991045,0.991045};
                                                               Double_t gfLambdaX [gfLambdaN]          =	{0.,1000};
                                                               Double_t gfLambdaY [gfLambdaN]          = {0.991045,0.991045};
                                                               Double_t gfKMinusX [gfKMinusN]          =	{0,0.54741,0.74137,1.03879,1.36207,1.96983,2.52586,3.0819,3.67672,4.19397,5.03448,5.44828,1000};
                                                               Double_t gfKMinusY [gfKMinusN]          = {0,0.979518,0.983133,0.987349,0.989759,0.992169,0.993976,0.996386,0.995783,0.998193,0.99759,1,1000};
                                                               fGeant3FlukaProton 	   = new TGraph(gfProtonN,gfProtonX,gfProtonY);
                                                               fGeant3FlukaAntiProton = new TGraph(gfAntiProtonN,gfAntiProtonX,gfAntiProtonY);
                                                               fGeant3FlukaLambda     = new TGraph(gfLambdaN,gfLambdaX,gfLambdaY);
                                                               fGeant3FlukaAntiLambda = new TGraph(gfAntiLambdaN,gfAntiLambdaX,gfAntiLambdaY);
                                                               fGeant3FlukaKMinus 	   = new TGraph(gfKMinusN,gfKMinusX,gfKMinusY);



  //General Information
  /*fh1dEventRejectionRDHFCuts = (TH1D*)AddHistogramm("fh1dEventRejectionRDHFCuts","fh1dEventRejectionRDHFCuts;reason;count",12,0,12);
  fh1dEventRejectionRDHFCuts->GetXaxis()->SetBinLabel(1,"Event accepted");
  fh1dEventRejectionRDHFCuts->GetXaxis()->SetBinLabel(2,"Event rejected");
  fh1dEventRejectionRDHFCuts->GetXaxis()->SetBinLabel(3,"Wrong physics selection");
  fh1dEventRejectionRDHFCuts->GetXaxis()->SetBinLabel(4,"No vertex");
  fh1dEventRejectionRDHFCuts->GetXaxis()->SetBinLabel(5,"No contributors");
  fh1dEventRejectionRDHFCuts->GetXaxis()->SetBinLabel(6,"Less than 10 contributors");
  fh1dEventRejectionRDHFCuts->GetXaxis()->SetBinLabel(7,">10cm vertex Z distance");
  fh1dEventRejectionRDHFCuts->GetXaxis()->SetBinLabel(8,"Bad diamond X distance");
  fh1dEventRejectionRDHFCuts->GetXaxis()->SetBinLabel(9,"Bad diamond Y distance");
  fh1dEventRejectionRDHFCuts->GetXaxis()->SetBinLabel(10,"Bad diamond Z distance");
  fh1dEventRejectionRDHFCuts->GetXaxis()->SetBinLabel(11,"Chi2 vtx >1.5 ");*/
  //AddHistogramm("fh1dTracksAccepeted","# tracks before/after cuts;;",3,0,3);

  //****************************************
  //QA Plots
  fh1dCutsPrinted =(TH1D*)AddHistogramm("fh1dCutsPrinted","",3,0,3);

  fh1dTracksAccepeted =(TH1D*)AddHistogramm("fh1dTracksAccepeted","# tracks before/after cuts;;",3,0,3);
  fh1dTracksAccepeted->GetXaxis()->SetBinLabel(1,"total");
  fh1dTracksAccepeted->GetXaxis()->SetBinLabel(2,"accepted");
  fh1dTracksAccepeted->GetXaxis()->SetBinLabel(3,"rejected");
  //Event Properties
  fHistManager.CreateTH1("fh1dNoParticlesPerEvent","fh1dNoParticlesvsEvent;#;No Particles/Event",5000, 0, 5000,"s");
  fHistManager.CreateTH1("fh1dNoJetsPerEvent","fh1dNoJetsPerEvent;#;No Jets/Event",400, 0, 100,"s");
  fHistManager.CreateTH1("fh1dEventsAcceptedInRun","fh1dEventsAcceptedInRun;Events Accepted;count",1,0,1,"s");
  fHistManager.CreateTH1("fh1dPtHardMonitor","fh1dPtHardMonitor;ptHard;",500,0,250,"s");
  //Jet Properties
  fHistManager.CreateTH2("fh1dJetRecEtaPhiAccepted","detector level jet;#eta;phi",1,-0.5,0.5,1,0.,TMath::TwoPi(),"s");
  fHistManager.CreateTH2("fh2dAcceptedTracksEtaPhi","accepted tracks;#eta;phi",200,-0.9,0.9,200,0.,TMath::TwoPi(),"s");
  fHistManager.CreateTH1("fh1dJetRecPt","detector level jets;pt (GeV/c); count",500,0,250,"s");
  fHistManager.CreateTH1("fh1dJetRecPtAccepted","accepted detector level jets;pt (GeV/c); count",500,0,250,"s");
  fHistManager.CreateTH1("fh1dJetArea","fh1dJetArea;# Jet Area",100,0,1,"s");
  fHistManager.CreateTH1("fh1dParticlesPerJet","fh1dParticlesPerJet;#, Particles/Jet",100,0,100,"s");
  fHistManager.CreateTH2("fh2dNoAcceptedTracksvsJetArea","fh2dNoAcceptedTracksvsJetArea;No Accepted Tracks;JetArea",20,0,20,100,0,1);
  //MC properties
  if(fIsPythia){
    /*fHistManager.CreateTH1("fh1dJetGenPt","generator level jets;pt (GeV/c); count",250,0,250,"s");
    fHistManager.CreateTH1("fh1dJetGenPtUnidentified","generator level jets (no flavour assigned);pt (GeV/c); count",250,0,250,"s");
    fHistManager.CreateTH1("fh1dJetGenPtudsg","generator level udsg jets;pt (GeV/c); count",250,0,250,"s");
    fHistManager.CreateTH1("fh1dJetGenPtc","generator level c jets;pt (GeV/c); count",250,0,250,"s");
    fHistManager.CreateTH1("fh1dJetGenPtb","generator level b jets;pt (GeV/c); count",250,0,250,"s");
    fHistManager.CreateTH1("fh1dJetGenPts","generator level s jets;pt (GeV/c); count",250,0,250,"s");
    fHistManager.CreateTH2("fh2dJetGenPtVsJetRecPt","detector momentum response;gen pt;rec pt",500,0,250,500,0,250,"s");*/
    fHistManager.CreateTH1("fh1dJetRecPtudsg","detector level jets;pt (GeV/c); count",250,0,250,"s");
    fHistManager.CreateTH1("fh1dJetRecPtUnidentified","detector level jets;pt (GeV/c); count",250,0,250,"s");
    fHistManager.CreateTH1("fh1dJetRecPtc","detector level jets;pt (GeV/c); count",250,0,250,"s");
    fHistManager.CreateTH1("fh1dJetRecPtb","detector level jets;pt (GeV/c); count",250,0,250,"s");
    fHistManager.CreateTH1("fh1dJetRecPts","detector level jets;pt (GeV/c); count",250,0,250,"s");
  }

  fh1DCutInclusive=(TH1D*)AddHistogramm("fh1DCutInclusive","fh1DCutInclusive",30,0,30);
  fh1dCutudg=(TH1D*)AddHistogramm("fh1dCutudg","fh1dCutudg",30,0,30);
  fh1dCutc=(TH1D*)AddHistogramm("fh1dCutc","fh1dCutc",30,0,30);
  fh1dCutb=(TH1D*)AddHistogramm("fh1dCutb","fh1dCutb",30,0,30);
  fh1dCuts=(TH1D*)AddHistogramm("fh1dCuts","fh1dCuts",30,0,30);
  fh1dCuts->GetXaxis()->LabelsOption("v");

  for(Int_t iBin = 0; iBin < 25; iBin++){
          fh1DCutInclusive->GetXaxis()->SetBinLabel(iBin + 1, BJetCuts[iBin].Data());
          if(fIsPythia){

                  fh1dCutudg->GetXaxis()->SetBinLabel(iBin + 1, BJetCuts[iBin].Data());
                  fh1dCutb->GetXaxis()->SetBinLabel(iBin + 1, BJetCuts[iBin].Data());
                  fh1dCutc->GetXaxis()->SetBinLabel(iBin + 1, BJetCuts[iBin].Data());
                  fh1dCuts->GetXaxis()->SetBinLabel(iBin + 1, BJetCuts[iBin].Data());
          }
  }

  //****************************************
  //Lund Plane
  const Int_t dimSpec   = 6;
  const Int_t nBinsSpec[6]     = {50,100,100,20,100,2};
  const Double_t lowBinSpec[6] = {0.,-10,0,0,0,0};
  const Double_t hiBinSpec[6]  = {5.,10.,100,20,100,2};
  fHLundIterative = new THnSparseF("fHLundIterative",
                  "LundIterativePlot [log(1/theta),log(z*theta),pTjet,algo]",
                  dimSpec,nBinsSpec,lowBinSpec,hiBinSpec);
  if(fDoLundPlane)fOutput->Add(fHLundIterative);


  //****************************************
  //Histograms for Probability Tagging
  if (fIsPythia){
    const char * tagtype[6] = {"Full","Single1st","Single2nd","Single3rd","Double","Triple"};
    for(int iThresh=0;iThresh<fNThresholds;iThresh++){
      for(int iType=0;iType<6;iType++){
        fHistManager.CreateTH1(Form("h1DTrueBTagged_%s_%0.2f",tagtype[iType],fFracs[iThresh]),";jet pt; #",500,0,250);
        fHistManager.CreateTH1(Form("h1DFalseCTagged_%s_%0.2f",tagtype[iType],fFracs[iThresh]),";jet pt; #",500,0,250);
        fHistManager.CreateTH1(Form("h1DFalseUDSGTagged_%s_%0.2f",tagtype[iType],fFracs[iThresh]),";jet pt; #",500,0,250);
      }
    }

    h2DProbDistsUnid=(TH2D*)AddHistogramm("h2DProbDistsUnid","h2DProbDistsUnid",200, 0, 1,500, 0, 250);
    h2DProbDistsudsg=(TH2D*)AddHistogramm("h2DProbDistsudsg","h2DProbDistsUDSG",200, 0, 1,500, 0, 250);
    h2DProbDistsc=(TH2D*)AddHistogramm("h2DProbDistsc","h2DProbDistsC",200, 0, 1,500, 0, 250);
    h2DProbDistsb=(TH2D*)AddHistogramm("h2DProbDistsb","h2DProbDistsB",200, 0, 1,500, 0, 250);
    h2DProbDistss=(TH2D*)AddHistogramm("h2DProbDistss","h2DProbDistsS",200, 0, 1,500, 0, 250);
    h2DProbDists=(TH2D*)AddHistogramm("h2DProbDists","h2DProbDistsAll",200, 0, 1,500, 0, 250);
  
    h2DLNProbDistsUnid=(TH2D*)AddHistogramm("h2DLNProbDistsUnid","h2DProbDistsUnid",200, 0, 1,500, 0, 250);
    h2DLNProbDistsudsg=(TH2D*)AddHistogramm("h2DLNProbDistsudsg","h2DProbDistsUDSG",200, 0, 1,500, 0, 250);
    h2DLNProbDistsc=(TH2D*)AddHistogramm("h2DLNProbDistsc","h2DProbDistsC",200, 0, 1,500, 0, 250);
    h2DLNProbDistsb=(TH2D*)AddHistogramm("h2DLNProbDistsb","h2DProbDistsB",200, 0, 1,500, 0, 250);
    h2DLNProbDistss=(TH2D*)AddHistogramm("h2DLNProbDistss","h2DProbDistsS",200, 0, 1,500, 0, 250);
    h2DLNProbDists=(TH2D*)AddHistogramm("h2DLNProbDists","h2DProbDistsAll",200, 0, 1,500, 0, 250);

  }
  for(int iThresh=0;iThresh<fNThresholds;iThresh++){
    fHistManager.CreateTH1(Form("h1DTagged_%0.2f",fFracs[iThresh]),";jet pt; #",500,0,250);
  }

  //****************************************
  //Corrections

  //MC weights
  /*fHistManager.CreateTH2("fh2dNMCWeightSpeciesPerJetPtN1_IP_all","fh2dNMCWeightSpeciesPerJetPtN1_all (N used weights) ;species i;pt",22,0,22,150,0,150,"s");
  fHistManager.CreateTH2("fh2dNMCWeightSpeciesPerJetPtN1_IP_b","fh2dNMCWeightSpeciesPerJetPtN1_b (N used weights) ;species i;pt",22,0,22,150,0,150,"s");
  fHistManager.CreateTH2("fh2dNMCWeightSpeciesPerJetPtN1_IP_c","fh2dNMCWeightSpeciesPerJetPtN1_c (N used weights) ;species i;pt",22,0,22,150,0,150,"s");
  fHistManager.CreateTH2("fh2dNMCWeightSpeciesPerJetPtN1_IP_lf","fh2dNMCWeightSpeciesPerJetPtN1_lf (N used weights) ;species i;pt",22,0,22,150,0,150,"s");
  fHistManager.CreateTH2("fh2dNMCWeightSpeciesPerJetPtN2_IP_all","fh2dNMCWeightSpeciesPerJetPtN2_all (N used weights) ;species i;pt",22,0,22,150,0,150,"s");
  fHistManager.CreateTH2("fh2dNMCWeightSpeciesPerJetPtN2_IP_b","fh2dNMCWeightSpeciesPerJetPtN2_b (N used weights) ;species i;pt",22,0,22,150,0,150,"s");
  fHistManager.CreateTH2("fh2dNMCWeightSpeciesPerJetPtN2_IP_c","fh2dNMCWeightSpeciesPerJetPtN2_c (N used weights) ;species i;pt",22,0,22,150,0,150,"s");
  fHistManager.CreateTH2("fh2dNMCWeightSpeciesPerJetPtN2_IP_lf","fh2dNMCWeightSpeciesPerJetPtN2_lf (N used weights) ;species i;pt",22,0,22,150,0,150,"s");
  fHistManager.CreateTH2("fh2dNMCWeightSpeciesPerJetPtN3_IP_all","fh2dNMCWeightSpeciesPerJetPtN3_all (N used weights) ;species i;pt",22,0,22,150,0,150,"s");
  fHistManager.CreateTH2("fh2dNMCWeightSpeciesPerJetPtN3_IP_b","fh2dNMCWeightSpeciesPerJetPtN3_b (N used weights) ;species i;pt",22,0,22,150,0,150,"s");
  fHistManager.CreateTH2("fh2dNMCWeightSpeciesPerJetPtN3_IP_c","fh2dNMCWeightSpeciesPerJetPtN3_c (N used weights) ;species i;pt",22,0,22,150,0,150,"s");
  fHistManager.CreateTH2("fh2dNMCWeightSpeciesPerJetPtN3_IP_lf","fh2dNMCWeightSpeciesPerJetPtN3_lf (N used weights) ;species i;pt",22,0,22,150,0,150,"s");
  fHistManager.CreateTH2("fh2dNMCWeightSpeciesPerJetPtN1_SIP_all","fh2dNMCWeightSpeciesPerJetPtN1_all (N used weights) ;species i;pt",22,0,22,150,0,150,"s");
  fHistManager.CreateTH2("fh2dNMCWeightSpeciesPerJetPtN1_SIP_b","fh2dNMCWeightSpeciesPerJetPtN1_b (N used weights) ;species i;pt",22,0,22,150,0,150,"s");
  fHistManager.CreateTH2("fh2dNMCWeightSpeciesPerJetPtN1_SIP_c","fh2dNMCWeightSpeciesPerJetPtN1_c (N used weights) ;species i;pt",22,0,22,150,0,150,"s");
  fHistManager.CreateTH2("fh2dNMCWeightSpeciesPerJetPtN1_SIP_lf","fh2dNMCWeightSpeciesPerJetPtN1_lf (N used weights) ;species i;pt",22,0,22,150,0,150,"s");
  fHistManager.CreateTH2("fh2dNMCWeightSpeciesPerJetPtN2_SIP_all","fh2dNMCWeightSpeciesPerJetPtN2_all (N used weights) ;species i;pt",22,0,22,150,0,150,"s");
  fHistManager.CreateTH2("fh2dNMCWeightSpeciesPerJetPtN2_SIP_b","fh2dNMCWeightSpeciesPerJetPtN2_b (N used weights) ;species i;pt",22,0,22,150,0,150,"s");
  fHistManager.CreateTH2("fh2dNMCWeightSpeciesPerJetPtN2_SIP_c","fh2dNMCWeightSpeciesPerJetPtN2_c (N used weights) ;species i;pt",22,0,22,150,0,150,"s");
  fHistManager.CreateTH2("fh2dNMCWeightSpeciesPerJetPtN2_SIP_lf","fh2dNMCWeightSpeciesPerJetPtN2_lf (N used weights) ;species i;pt",22,0,22,150,0,150,"s");
  fHistManager.CreateTH2("fh2dNMCWeightSpeciesPerJetPtN3_SIP_all","fh2dNMCWeightSpeciesPerJetPtN3_all (N used weights) ;species i;pt",22,0,22,150,0,150,"s");
  fHistManager.CreateTH2("fh2dNMCWeightSpeciesPerJetPtN3_SIP_b","fh2dNMCWeightSpeciesPerJetPtN3_b (N used weights) ;species i;pt",22,0,22,150,0,150,"s");
  fHistManager.CreateTH2("fh2dNMCWeightSpeciesPerJetPtN3_SIP_c","fh2dNMCWeightSpeciesPerJetPtN3_c (N used weights) ;species i;pt",22,0,22,150,0,150,"s");
  fHistManager.CreateTH2("fh2dNMCWeightSpeciesPerJetPtN3_SIP_lf","fh2dNMCWeightSpeciesPerJetPtN3_lf (N used weights) ;species i;pt",22,0,22,150,0,150,"s");*/
                                                            
  //Check N1N2N3 correlations
  /*if(fUseTreeForCorrelations){
    Printf("Adding Tree to output file");
    OpenFile(1);
    fCorrelationCrossCheck = new TTree("fCorrelationCrossCheck","fCorrelationCrossCheck");
    fCorrelationCrossCheck->Branch("n1",&fTREE_n1,"px/F");
    fCorrelationCrossCheck->Branch("n2",&fTREE_n2,"py/F");
    fCorrelationCrossCheck->Branch("n3",&fTREE_n3,"pz/F");
    fCorrelationCrossCheck->Branch("pt",&fTREE_pt,"pz/F");
    fOutput->Add(fCorrelationCrossCheck);
  }*/

  /*if (fFillCorrelations && !fUseTreeForCorrelations){
    fHistManager.CreateTH2("fh2dInclusiveCorrelationN1N2","fh2dInclusiveCorrelationN1N2 ;N1 impact parameter xy (cm);N2impact parameter xy (cm)",1000,-50,50,1000,-50,50,"s");
    fHistManager.CreateTH2("fh2dInclusiveCorrelationN1N3","fh2dInclusiveCorrelationN1N3 ;N1 impact parameter xy (cm);N3impact parameter xy (cm)",1000,-50,50,1000,-50,50,"s");
    fHistManager.CreateTH2("fh2dInclusiveCorrelationN2N3","fh2dInclusiveCorrelationN2N3 ;N2 impact parameter xy (cm);N3impact parameter xy (cm)",1000,-50,50,1000,-50,50,"s");
    fHistManager.CreateTH2("fh2dGreater10_20GeVCorrelationN1N2","fh2dGreater10_20GeVCorrelationN1N2 ;N1 impact parameter xy (cm);N2impact parameter xy (cm)",1000,-50,50,1000,-50,50,"s");
    fHistManager.CreateTH2("fh2dGreater10_20GeVCorrelationN1N3","fh2dGreater10_20GeVCorrelationN1N3 ;N1 impact parameter xy (cm);N3impact parameter xy (cm)",1000,-50,50,1000,-50,50,"s");
    fHistManager.CreateTH2("fh2dGreater10_20GeVCorrelationN2N3","fh2dGreater10_20GeVCorrelationN2N3 ;N2 impact parameter xy (cm);N3impact parameter xy (cm)",1000,-50,50,1000,-50,50,"s");
    fHistManager.CreateTH2("fh2dGreater20_30GeVCorrelationN1N2","fh2dGreater20_30GeVCorrelationN1N2 ;N1 impact parameter xy (cm);N2impact parameter xy (cm)",1000,-50,50,1000,-50,50,"s");
    fHistManager.CreateTH2("fh2dGreater20_30GeVCorrelationN1N3","fh2dGreater20_30GeVCorrelationN1N3 ;N1 impact parameter xy (cm);N3impact parameter xy (cm)",1000,-50,50,1000,-50,50,"s");
    fHistManager.CreateTH2("fh2dGreater20_30GeVCorrelationN2N3","fh2dGreater20_30GeVCorrelationN2N3 ;N2 impact parameter xy (cm);N3impact parameter xy (cm)",1000,-50,50,1000,-50,50,"s");
    fHistManager.CreateTH2("fh2dGreater30_100GeVCorrelationN1N2","fh2dGreater30_100GeVCorrelationN1N2 ;N1 impact parameter xy (cm);N2impact parameter xy (cm)",1000,-50,50,1000,-50,50,"s");
    fHistManager.CreateTH2("fh2dGreater30_100GeVCorrelationN1N3","fh2dGreater30_100GeVCorrelationN1N3 ;N1 impact parameter xy (cm);N3impact parameter xy (cm)",1000,-50,50,1000,-50,50,"s");
    fHistManager.CreateTH2("fh2dGreater30_100GeVCorrelationN2N3","fh2dGreater30_100GeVCorrelationN2N3 ;N2 impact parameter xy (cm);N3impact parameter xy (cm)",1000,-50,50,1000,-50,50,"s");
    fHistManager.CreateTH2("fh2dInclusiveCorrelationN1N2mix","fh2dInclusiveCorrelationN1N2mix ;N1 impact parameter xy (cm);N2impact parameter xy (cm)",1000,-50,50,1000,-50,50,"s");
    fHistManager.CreateTH2("fh2dInclusiveCorrelationN1N3mix","fh2dInclusiveCorrelationN1N3mix ;N1 impact parameter xy (cm);N3impact parameter xy (cm)",1000,-50,50,1000,-50,50,"s");
    fHistManager.CreateTH2("fh2dInclusiveCorrelationN2N3mix","fh2dInclusiveCorrelationN2N3mix ;N2 impact parameter xy (cm);N3impact parameter xy (cm)",1000,-50,50,1000,-50,50,"s");
    fHistManager.CreateTH2("fh2dGreater10_20GeVCorrelationN1N2mix","fh2dGreater10_20GeVCorrelationN1N2mix ;N1 impact parameter xy (cm);N2impact parameter xy (cm)",1000,-50,50,1000,-50,50,"s");
    fHistManager.CreateTH2("fh2dGreater10_20GeVCorrelationN1N3mix","fh2dGreater10_20GeVCorrelationN1N3mix ;N1 impact parameter xy (cm);N3impact parameter xy (cm)",1000,-50,50,1000,-50,50,"s");
    fHistManager.CreateTH2("fh2dGreater10_20GeVCorrelationN2N3mix","fh2dGreater10_20GeVCorrelationN2N3mix ;N2 impact parameter xy (cm);N3impact parameter xy (cm)",1000,-50,50,1000,-50,50,"s");
    fHistManager.CreateTH2("fh2dGreater20_30GeVCorrelationN1N2mix","fh2dGreater20_30GeVCorrelationN1N2mix ;N1 impact parameter xy (cm);N2impact parameter xy (cm)",1000,-50,50,1000,-50,50,"s");
    fHistManager.CreateTH2("fh2dGreater20_30GeVCorrelationN1N3mix","fh2dGreater20_30GeVCorrelationN1N3mix ;N1 impact parameter xy (cm);N3impact parameter xy (cm)",1000,-50,50,1000,-50,50,"s");
    fHistManager.CreateTH2("fh2dGreater20_30GeVCorrelationN2N3mix","fh2dGreater20_30GeVCorrelationN2N3mix ;N2 impact parameter xy (cm);N3impact parameter xy (cm)",1000,-50,50,1000,-50,50,"s");
    fHistManager.CreateTH2("fh2dGreater30_100GeVCorrelationN1N2mix","fh2dGreater30_100GeVCorrelationN1N2mix ;N1 impact parameter xy (cm);N2impact parameter xy (cm)",1000,-50,50,1000,-50,50,"s");
    fHistManager.CreateTH2("fh2dGreater30_100GeVCorrelationN1N3mix","fh2dGreater30_100GeVCorrelationN1N3mix ;N1 impact parameter xy (cm);N3impact parameter xy (cm)",1000,-50,50,1000,-50,50,"s");
    fHistManager.CreateTH2("fh2dGreater30_100GeVCorrelationN2N3mix","fh2dGreater30_100GeVCorrelationN2N3mix ;N2 impact parameter xy (cm);N3impact parameter xy (cm)",1000,-50,50,1000,-50,50,"s");
  }*/

    //****************************************
    //Track Impact Parameter Distributions
    fHistManager.CreateTH2("fh2dTracksImpParXY","radial imp. parameter ;impact parameter xy (cm);a.u.",2000,lowIPxy,highIPxy,500,0,100.,"s");
    fHistManager.CreateTH2("fh2dTracksImpParXYZ","XYZ imp. parameter ;impact parameter xy (cm);a.u.",2000,-1,1,500,0,100.,"s");
    //fHistManager.CreateTH2("fh2dTracksImpParXYZSignificance","XYZ imp. parameter ;impact parameter xy (cm);a.u.",2000,-30,30,500,0,100.,"s");
    fHistManager.CreateTH2("fh2dTracksImpParZ","z imp. parameter ;impact parameter xy (cm);a.u.",2000,lowIPxy,highIPxy,500,0,10.,"s");
    fHistManager.CreateTH2("fh2dTracksImpParXYSignificance","radial imp. parameter sig;impact parameter xy (cm);a.u.",2000,-30,30,500,0,100.,"s");
    fHistManager.CreateTH2("fh2dTracksImpParZSignificance","z imp. parameter ;impact parameter xy (cm);a.u.",2000,-30,30,500,0,100.,"s");
    fHistManager.CreateTH1("fh1dTracksImpParXY","2d imp. parameter ;impact parameter 2d (cm);a.u.",400,-0.2,0.2,"s");
    fHistManager.CreateTH1("fh1dTracksImpParXYZ","3d imp. parameter ;impact parameter 3d (cm);a.u.",2000,0,1.,"s");
    fHistManager.CreateTH1("fh1dTracksImpParXYSignificance","radial imp. parameter ;impact parameter xy significance;a.u.",200,-30,30,"s");
    //fHistManager.CreateTH1 ("fh1dTracksImpParXYZSignificance","3d imp. parameter ;impact parameter 3d significance;a.u.",2000,0.,100.,"s");

    //****************************************
    //Pt Distributions for N1,N2,N3 Tracks
    if(fIsPythia){
      fHistManager.CreateTH1("fh1dJetRecPt_n_1_b_Accepted","detector level jets;pt (GeV/c); count",500,0,250,"s");
      fHistManager.CreateTH1("fh1dJetRecPt_n_2_b_Accepted","detector level jets;pt (GeV/c); count",500,0,250,"s");
      fHistManager.CreateTH1("fh1dJetRecPt_n_3_b_Accepted","detector level jets;pt (GeV/c); count",500,0,250,"s");

      fHistManager.CreateTH1("fh1dJetRecPt_n_1_c_Accepted","detector level jets;pt (GeV/c); count",500,0,250,"s");
      fHistManager.CreateTH1("fh1dJetRecPt_n_2_c_Accepted","detector level jets;pt (GeV/c); count",500,0,250,"s");
      fHistManager.CreateTH1("fh1dJetRecPt_n_3_c_Accepted","detector level jets;pt (GeV/c); count",500,0,250,"s");

      fHistManager.CreateTH1("fh1dJetRecPt_n_1_udsg_Accepted","detector level jets;pt (GeV/c); count",500,0,250,"s");
      fHistManager.CreateTH1("fh1dJetRecPt_n_2_udsg_Accepted","detector level jets;pt (GeV/c); count",500,0,250,"s");
      fHistManager.CreateTH1("fh1dJetRecPt_n_3_udsg_Accepted","detector level jets;pt (GeV/c); count",500,0,250,"s");

      fHistManager.CreateTH1("fh1dJetRecPt_n_1_s_Accepted","detector level jets;pt (GeV/c); count",500,0,250,"s");
      fHistManager.CreateTH1("fh1dJetRecPt_n_2_s_Accepted","detector level jets;pt (GeV/c); count",500,0,250,"s");
      fHistManager.CreateTH1("fh1dJetRecPt_n_3_s_Accepted","detector level jets;pt (GeV/c); count",500,0,250,"s");
    }
    fHistManager.CreateTH1("fh1dJetRecPt_n_1_all_Accepted","detector level jets;pt (GeV/c); count",500,0,250,"s");
    fHistManager.CreateTH1("fh1dJetRecPt_n_2_all_Accepted","detector level jets;pt (GeV/c); count",500,0,250,"s");
    fHistManager.CreateTH1("fh1dJetRecPt_n_3_all_Accepted","detector level jets;pt (GeV/c); count",500,0,250,"s");
    fHistManager.CreateTH1("fh1dTrackPt_n_1_all_Accepted","detector level jets;pt (GeV/c); count",500,0,200,"s");
    fHistManager.CreateTH1("fh1dTrackPt_n_2_all_Accepted","detector level jets;pt (GeV/c); count",500,0,200,"s");
    fHistManager.CreateTH1("fh1dTrackPt_n_3_all_Accepted","detector level jets;pt (GeV/c); count",500,0,200,"s");



    //IP Distributions for different species for different numbers of ITShits
    //if (fIsPythia){
    /*  fHistManager.CreateTH2("fh2dJetSignedImpParXYSignificanceudg_light_resfunction_6ITShits", "fh2dJetSignedImpParXYSignificanceudg_light_resfunction_6ITShits;pt (GeV/c); count",200,0,100,500,-30,0,"s");
      fHistManager.CreateTH2("fh2dJetSignedImpParXYZSignificanceudg_light_resfunction_6ITShits","fh2dJetSignedImpParXYZSignificanceudg_light_resfunction_6ITShits;pt (GeV/c); count",200,0,100,500,-30,0,"s");
      fHistManager.CreateTH2("fh2dJetSignedImpParXYSignificanceudg_light_resfunction_5ITShits", "fh2dJetSignedImpParXYSignificanceudg_light_resfunction_5ITShits;pt (GeV/c); count",200,0,100,500,-30,0,"s");
      fHistManager.CreateTH2("fh2dJetSignedImpParXYZSignificanceudg_light_resfunction_5ITShits","fh2dJetSignedImpParXYZSignificanceudg_light_resfunction_5ITShits;pt (GeV/c); count",200,0,100,500,-30,0,"s");
      fHistManager.CreateTH2("fh2dJetSignedImpParXYSignificanceudg_light_resfunction_4ITShits", "fh2dJetSignedImpParXYSignificanceudg_light_resfunction_4ITShits;pt (GeV/c); count",200,0,100,500,-30,0,"s");
      fHistManager.CreateTH2("fh2dJetSignedImpParXYZSignificanceudg_light_resfunction_4ITShits","fh2dJetSignedImpParXYZSignificanceudg_light_resfunction_4ITShits;pt (GeV/c); count",200,0,100,500,-30,0,"s");
      fHistManager.CreateTH2("fh2dJetSignedImpParXYSignificanceudg_light_resfunction_3ITShits", "fh2dJetSignedImpParXYSignificanceudg_light_resfunction_3ITShits;pt (GeV/c); count",200,0,100,500,-30,0,"s");
      fHistManager.CreateTH2("fh2dJetSignedImpParXYZSignificanceudg_light_resfunction_3ITShits","fh2dJetSignedImpParXYZSignificanceudg_light_resfunction_3ITShits;pt (GeV/c); count",200,0,100,500,-30,0,"s");
            //Electrons
      fHistManager.CreateTH2("fh2dJetSignedImpParXYSignificanceudg_light_resfunction_6ITShitsElectrons", "fh2dJetSignedImpParXYSignificanceudg_light_resfunction_6ITShitsElectrons;pt (GeV/c); count",200,0,100,500,-30,0,"s");
      fHistManager.CreateTH2("fh2dJetSignedImpParXYZSignificanceudg_light_resfunction_6ITShitsElectrons","fh2dJetSignedImpParXYZSignificanceudg_light_resfunction_6ITShitsElectrons;pt (GeV/c); count",200,0,100,500,-30,0,"s");
      fHistManager.CreateTH2("fh2dJetSignedImpParXYSignificanceudg_light_resfunction_5ITShitsElectrons", "fh2dJetSignedImpParXYSignificanceudg_light_resfunction_5ITShitsElectrons;pt (GeV/c); count",200,0,100,500,-30,0,"s");
      fHistManager.CreateTH2("fh2dJetSignedImpParXYZSignificanceudg_light_resfunction_5ITShitsElectrons","fh2dJetSignedImpParXYZSignificanceudg_light_resfunction_5ITShitsElectrons;pt (GeV/c); count",200,0,100,500,-30,0,"s");
      fHistManager.CreateTH2("fh2dJetSignedImpParXYSignificanceudg_light_resfunction_4ITShitsElectrons", "fh2dJetSignedImpParXYSignificanceudg_light_resfunction_4ITShitsElectrons;pt (GeV/c); count",200,0,100,500,-30,0,"s");
      fHistManager.CreateTH2("fh2dJetSignedImpParXYZSignificanceudg_light_resfunction_4ITShitsElectrons","fh2dJetSignedImpParXYZSignificanceudg_light_resfunction_4ITShitsElectrons;pt (GeV/c); count",200,0,100,500,-30,0,"s");
      fHistManager.CreateTH2("fh2dJetSignedImpParXYSignificanceudg_light_resfunction_3ITShitsElectrons", "fh2dJetSignedImpParXYSignificanceudg_light_resfunction_3ITShitsElectrons;pt (GeV/c); count",200,0,100,500,-30,0,"s");
      fHistManager.CreateTH2("fh2dJetSignedImpParXYZSignificanceudg_light_resfunction_3ITShitsElectrons","fh2dJetSignedImpParXYZSignificanceudg_light_resfunction_3ITShitsElectrons;pt (GeV/c); count",200,0,100,500,-30,0,"s");
            //Pions
      fHistManager.CreateTH2("fh2dJetSignedImpParXYSignificanceudg_light_resfunction_6ITShitsPions", "fh2dJetSignedImpParXYSignificanceudg_light_resfunction_6ITShitsPions;pt (GeV/c); count",200,0,100,500,-30,0,"s");
      fHistManager.CreateTH2("fh2dJetSignedImpParXYZSignificanceudg_light_resfunction_6ITShitsPions","fh2dJetSignedImpParXYZSignificanceudg_light_resfunction_6ITShitsPions;pt (GeV/c); count",200,0,100,500,-30,0,"s");
      fHistManager.CreateTH2("fh2dJetSignedImpParXYSignificanceudg_light_resfunction_5ITShitsPions", "fh2dJetSignedImpParXYSignificanceudg_light_resfunction_5ITShitsPions;pt (GeV/c); count",200,0,100,500,-30,0,"s");
      fHistManager.CreateTH2("fh2dJetSignedImpParXYZSignificanceudg_light_resfunction_5ITShitsPions","fh2dJetSignedImpParXYZSignificanceudg_light_resfunction_5ITShitsPions;pt (GeV/c); count",200,0,100,500,-30,0,"s");
      fHistManager.CreateTH2("fh2dJetSignedImpParXYSignificanceudg_light_resfunction_4ITShitsPions", "fh2dJetSignedImpParXYSignificanceudg_light_resfunction_4ITShitsPions;pt (GeV/c); count",200,0,100,500,-30,0,"s");
      fHistManager.CreateTH2("fh2dJetSignedImpParXYZSignificanceudg_light_resfunction_4ITShitsPions","fh2dJetSignedImpParXYZSignificanceudg_light_resfunction_4ITShitsPions;pt (GeV/c); count",200,0,100,500,-30,0,"s");
      fHistManager.CreateTH2("fh2dJetSignedImpParXYSignificanceudg_light_resfunction_3ITShitsPions", "fh2dJetSignedImpParXYSignificanceudg_light_resfunction_3ITShitsPions;pt (GeV/c); count",200,0,100,500,-30,0,"s");
      fHistManager.CreateTH2("fh2dJetSignedImpParXYZSignificanceudg_light_resfunction_3ITShitsPions","fh2dJetSignedImpParXYZSignificanceudg_light_resfunction_3ITShitsPions;pt (GeV/c); count",200,0,100,500,-30,0,"s");
            //Kaons
      fHistManager.CreateTH2("fh2dJetSignedImpParXYSignificanceudg_light_resfunction_6ITShitsKaons", "fh2dJetSignedImpParXYSignificanceudg_light_resfunction_6ITShitsKaons;pt (GeV/c); count",200,0,100,500,-30,0,"s");
      fHistManager.CreateTH2("fh2dJetSignedImpParXYZSignificanceudg_light_resfunction_6ITShitsKaons","fh2dJetSignedImpParXYZSignificanceudg_light_resfunction_6ITShitsKaons;pt (GeV/c); count",200,0,100,500,-30,0,"s");
      fHistManager.CreateTH2("fh2dJetSignedImpParXYSignificanceudg_light_resfunction_5ITShitsKaons", "fh2dJetSignedImpParXYSignificanceudg_light_resfunction_5ITShitsKaons;pt (GeV/c); count",200,0,100,500,-30,0,"s");
      fHistManager.CreateTH2("fh2dJetSignedImpParXYZSignificanceudg_light_resfunction_5ITShitsKaons","fh2dJetSignedImpParXYZSignificanceudg_light_resfunction_5ITShitsKaons;pt (GeV/c); count",200,0,100,500,-30,0,"s");
      fHistManager.CreateTH2("fh2dJetSignedImpParXYSignificanceudg_light_resfunction_4ITShitsKaons", "fh2dJetSignedImpParXYSignificanceudg_light_resfunction_4ITShitsKaons;pt (GeV/c); count",200,0,100,500,-30,0,"s");
      fHistManager.CreateTH2("fh2dJetSignedImpParXYZSignificanceudg_light_resfunction_4ITShitsKaons","fh2dJetSignedImpParXYZSignificanceudg_light_resfunction_4ITShitsKaons;pt (GeV/c); count",200,0,100,500,-30,0,"s");
      fHistManager.CreateTH2("fh2dJetSignedImpParXYSignificanceudg_light_resfunction_3ITShitsKaons", "fh2dJetSignedImpParXYSignificanceudg_light_resfunction_3ITShitsKaons;pt (GeV/c); count",200,0,100,500,-30,0,"s");
      fHistManager.CreateTH2("fh2dJetSignedImpParXYZSignificanceudg_light_resfunction_3ITShitsKaons","fh2dJetSignedImpParXYZSignificanceudg_light_resfunction_3ITShitsKaons;pt (GeV/c); count",200,0,100,500,-30,0,"s");
            //Protons
      fHistManager.CreateTH2("fh2dJetSignedImpParXYSignificanceudg_light_resfunction_6ITShitsProtons", "fh2dJetSignedImpParXYSignificanceudg_light_resfunction_6ITShitsProtons;pt (GeV/c); count",200,0,100,500,-30,0,"s");
      fHistManager.CreateTH2("fh2dJetSignedImpParXYZSignificanceudg_light_resfunction_6ITShitsProtons","fh2dJetSignedImpParXYZSignificanceudg_light_resfunction_6ITShitsProtons;pt (GeV/c); count",200,0,100,500,-30,0,"s");
      fHistManager.CreateTH2("fh2dJetSignedImpParXYSignificanceudg_light_resfunction_5ITShitsProtons", "fh2dJetSignedImpParXYSignificanceudg_light_resfunction_5ITShitsProtons;pt (GeV/c); count",200,0,100,500,-30,0,"s");
      fHistManager.CreateTH2("fh2dJetSignedImpParXYZSignificanceudg_light_resfunction_5ITShitsProtons","fh2dJetSignedImpParXYZSignificanceudg_light_resfunction_5ITShitsProtons;pt (GeV/c); count",200,0,100,500,-30,0,"s");
      fHistManager.CreateTH2("fh2dJetSignedImpParXYSignificanceudg_light_resfunction_4ITShitsProtons", "fh2dJetSignedImpParXYSignificanceudg_light_resfunction_4ITShitsProtons;pt (GeV/c); count",200,0,100,500,-30,0,"s");
      fHistManager.CreateTH2("fh2dJetSignedImpParXYZSignificanceudg_light_resfunction_4ITShitsProtons","fh2dJetSignedImpParXYZSignificanceudg_light_resfunction_4ITShitsProtons;pt (GeV/c); count",200,0,100,500,-30,0,"s");
      fHistManager.CreateTH2("fh2dJetSignedImpParXYSignificanceudg_light_resfunction_3ITShitsProtons", "fh2dJetSignedImpParXYSignificanceudg_light_resfunction_3ITShitsProtons;pt (GeV/c); count",200,0,100,500,-30,0,"s");
      fHistManager.CreateTH2("fh2dJetSignedImpParXYZSignificanceudg_light_resfunction_3ITShitsProtons","fh2dJetSignedImpParXYZSignificanceudg_light_resfunction_3ITShitsProtons;pt (GeV/c); count",200,0,100,500,-30,0,"s");

      //Jet Probability QA  plots for different particle species
      if(fDoJetProb){
         fHistManager.CreateTH2("fh2d_jetprob_beauty", "fh2d_jetprob_beauty;pt (GeV/c); count",500,0,250,500,0,2.5,"s");
         fHistManager.CreateTH2("fh2d_jetprob_charm", "fh2d_jetprob_charm;pt (GeV/c); count",500,0,250,500,0,2.5,"s");
         fHistManager.CreateTH2("fh2d_jetprob_light", "fh2d_jetprob_light;pt (GeV/c); count",500,0,250,500,0,2.5,"s");
            //Templates for different probabilities
            //50%
         fHistManager.CreateTH2("fh2d_ImpSigXY_b_0_5JP", "fh2d_ImpSigXY_b_0_5JP;pt (GeV/c); sig",500,0,250,1000,-30,30,"s");
         fHistManager.CreateTH2("fh2d_ImpSigXYZ_b_0_5JP", "fh2d_ImpSigXYZ_b_0_5JP;pt (GeV/c); sig",500,0,250,1000,-30,30,"s");
         fHistManager.CreateTH2("fh2d_ImpSigXY_c_0_5JP", "fh2d_ImpSigXY_c_0_5JP;pt (GeV/c); sig",500,0,250,1000,-30,30,"s");
         fHistManager.CreateTH2("fh2d_ImpSigXYZ_c_0_5JP", "fh2d_ImpSigXYZ_c_0_5JP;pt (GeV/c); sig",500,0,250,1000,-30,30,"s");
         fHistManager.CreateTH2("fh2d_ImpSigXY_udsg_0_5JP", "fh2d_ImpSigXY_udsg_0_5JP;pt (GeV/c); sig",500,0,250,1000,-30,30,"s");
         fHistManager.CreateTH2("fh2d_ImpSigXYZ_udsg_0_5JP", "fh2d_ImpSigXYZ_udsg_0_5JP;pt (GeV/c); sig",500,0,250,1000,-30,30,"s");
         fHistManager.CreateTH1("fh1dJetRecPt_0_5JP_bAccepted","detector level jets;pt (GeV/c); count",500,0,250,"s");
         fHistManager.CreateTH1("fh1dJetRecPt_0_5JP_cAccepted","detector level jets;pt (GeV/c); count",500,0,250,"s");
         fHistManager.CreateTH1("fh1dJetRecPt_0_5JP_udsgAccepted","detector level jets;pt (GeV/c); count",500,0,250,"s");
            //60%
         fHistManager.CreateTH2("fh2d_ImpSigXY_b_0_6JP", "fh2d_ImpSigXY_b_0_6JP;pt (GeV/c); sig",500,0,250,1000,-30,30,"s");
         fHistManager.CreateTH2("fh2d_ImpSigXYZ_b_0_6JP", "fh2d_ImpSigXYZ_b_0_6JP;pt (GeV/c); sig",500,0,250,1000,-30,30,"s");
         fHistManager.CreateTH2("fh2d_ImpSigXY_c_0_6JP", "fh2d_ImpSigXY_c_0_5JP;pt (GeV/c); sig",500,0,250,1000,-30,30,"s");
         fHistManager.CreateTH2("fh2d_ImpSigXYZ_c_0_6JP", "fh2d_ImpSigXYZ_c_0_6JP;pt (GeV/c); sig",500,0,250,1000,-30,30,"s");
         fHistManager.CreateTH2("fh2d_ImpSigXY_udsg_0_6JP", "fh2d_ImpSigXY_udsg_0_6JP;pt (GeV/c); sig",500,0,250,1000,-30,30,"s");
         fHistManager.CreateTH2("fh2d_ImpSigXYZ_udsg_0_6JP", "fh2d_ImpSigXYZ_udsg_0_6JP;pt (GeV/c); sig",500,0,250,1000,-30,30,"s");
         fHistManager.CreateTH1("fh1dJetRecPt_0_6JP_bAccepted","detector level jets;pt (GeV/c); count",500,0,250,"s");
         fHistManager.CreateTH1("fh1dJetRecPt_0_6JP_cAccepted","detector level jets;pt (GeV/c); count",500,0,250,"s");
         fHistManager.CreateTH1("fh1dJetRecPt_0_6JP_udsgAccepted","detector level jets;pt (GeV/c); count",500,0,250,"s");
            //70%
         fHistManager.CreateTH2("fh2d_ImpSigXY_b_0_7JP", "fh2d_ImpSigXY_b_0_7JP;pt (GeV/c); sig",500,0,250,1000,-30,30,"s");
         fHistManager.CreateTH2("fh2d_ImpSigXYZ_b_0_7JP", "fh2d_ImpSigXYZ_b_0_7JP;pt (GeV/c); sig",500,0,250,1000,-30,30,"s");
         fHistManager.CreateTH2("fh2d_ImpSigXY_c_0_7JP", "fh2d_ImpSigXY_c_0_7JP;pt (GeV/c); sig",500,0,250,1000,-30,30,"s");
         fHistManager.CreateTH2("fh2d_ImpSigXYZ_c_0_7JP", "fh2d_ImpSigXYZ_c_0_7JP;pt (GeV/c); sig",500,0,250,1000,-30,30,"s");
         fHistManager.CreateTH2("fh2d_ImpSigXY_udsg_0_7JP", "fh2d_ImpSigXY_udsg_0_7JP;pt (GeV/c); sig",500,0,250,1000,-30,30,"s");
         fHistManager.CreateTH2("fh2d_ImpSigXYZ_udsg_0_7JP", "fh2d_ImpSigXYZ_udsg_0_7JP;pt (GeV/c); sig",500,0,250,1000,-30,30,"s");
         fHistManager.CreateTH1("fh1dJetRecPt_0_7JP_bAccepted","detector level jets;pt (GeV/c); count",500,0,250,"s");
         fHistManager.CreateTH1("fh1dJetRecPt_0_7JP_cAccepted","detector level jets;pt (GeV/c); count",500,0,250,"s");
         fHistManager.CreateTH1("fh1dJetRecPt_0_7JP_udsgAccepted","detector level jets;pt (GeV/c); count",500,0,250,"s");
            //80%
         fHistManager.CreateTH2("fh2d_ImpSigXY_b_0_8JP", "fh2d_ImpSigXY_b_0_8JP;pt (GeV/c); sig",500,0,250,1000,-30,30,"s");
         fHistManager.CreateTH2("fh2d_ImpSigXYZ_b_0_8JP", "fh2d_ImpSigXYZ_b_0_8JP;pt (GeV/c); sig",500,0,250,1000,-30,30,"s");
         fHistManager.CreateTH2("fh2d_ImpSigXY_c_0_8JP", "fh2d_ImpSigXY_c_0_8JP;pt (GeV/c); sig",500,0,250,1000,-30,30,"s");
         fHistManager.CreateTH2("fh2d_ImpSigXYZ_c_0_8JP", "fh2d_ImpSigXYZ_c_0_8JP;pt (GeV/c); sig",500,0,250,1000,-30,30,"s");
         fHistManager.CreateTH2("fh2d_ImpSigXY_udsg_0_8JP", "fh2d_ImpSigXY_udsg_0_8JP;pt (GeV/c); sig",500,0,250,1000,-30,30,"s");
         fHistManager.CreateTH2("fh2d_ImpSigXYZ_udsg_0_8JP", "fh2d_ImpSigXYZ_udsg_0_8JP;pt (GeV/c); sig",500,0,250,1000,-30,30,"s");
         fHistManager.CreateTH1("fh1dJetRecPt_0_8JP_bAccepted","detector level jets;pt (GeV/c); count",500,0,250,"s");
         fHistManager.CreateTH1("fh1dJetRecPt_0_8JP_cAccepted","detector level jets;pt (GeV/c); count",500,0,250,"s");
         fHistManager.CreateTH1("fh1dJetRecPt_0_8JP_udsgAccepted","detector level jets;pt (GeV/c); count",500,0,250,"s");
            //90%
         fHistManager.CreateTH2("fh2d_ImpSigXY_b_0_9JP", "fh2d_ImpSigXY_b_0_9JP;pt (GeV/c); sig",500,0,250,1000,-30,30,"s");
         fHistManager.CreateTH2("fh2d_ImpSigXYZ_b_0_9JP", "fh2d_ImpSigXYZ_b_0_9JP;pt (GeV/c); sig ",500,0,250,1000,-30,30,"s");
         fHistManager.CreateTH2("fh2d_ImpSigXY_c_0_9JP", "fh2d_ImpSigXY_c_0_9JP;pt (GeV/c); sig",500,0,250,1000,-30,30,"s");
         fHistManager.CreateTH2("fh2d_ImpSigXYZ_c_0_9JP", "fh2d_ImpSigXYZ_c_0_9JP;pt (GeV/c); sig",500,0,250,1000,-30,30,"s");
         fHistManager.CreateTH2("fh2d_ImpSigXY_udsg_0_9JP", "fh2d_ImpSigXY_udsg_0_9JP;pt (GeV/c); sig",500,0,250,1000,-30,30,"s");
         fHistManager.CreateTH2("fh2d_ImpSigXYZ_udsg_0_9JP", "fh2d_ImpSigXYZ_udsg_0_9JP;pt (GeV/c); sig",500,0,250,1000,-30,30,"s");
         fHistManager.CreateTH1("fh1dJetRecPt_0_9JP_bAccepted","detector level jets;pt (GeV/c); count",500,0,250,"s");
         fHistManager.CreateTH1("fh1dJetRecPt_0_9JP_cAccepted","detector level jets;pt (GeV/c); count",500,0,250,"s");
         fHistManager.CreateTH1("fh1dJetRecPt_0_9JP_udsgAccepted","detector level jets;pt (GeV/c); count",500,0,250,"s");
            //95%
         fHistManager.CreateTH2("fh2d_ImpSigXY_b_0_95JP", "fh2d_ImpSigXY_b_0_95JP;pt (GeV/c); sig",500,0,250,1000,-30,30,"s");
         fHistManager.CreateTH2("fh2d_ImpSigXYZ_b_0_95JP", "fh2d_ImpSigXYZ_b_0_95JP;pt (GeV/c); sig ",500,0,250,1000,-30,30,"s");
         fHistManager.CreateTH2("fh2d_ImpSigXY_c_0_95JP", "fh2d_ImpSigXY_c_0_95JP;pt (GeV/c); sig",500,0,250,1000,-30,30,"s");
         fHistManager.CreateTH2("fh2d_ImpSigXYZ_c_0_95JP", "fh2d_ImpSigXYZ_c_0_95JP;pt (GeV/c); sig",500,0,250,1000,-30,30,"s");
         fHistManager.CreateTH2("fh2d_ImpSigXY_udsg_0_95JP", "fh2d_ImpSigXY_udsg_0_95JP;pt (GeV/c); sig",500,0,250,1000,-30,30,"s");
         fHistManager.CreateTH2("fh2d_ImpSigXYZ_udsg_0_95JP", "fh2d_ImpSigXYZ_udsg_0_95JP;pt (GeV/c); sig",500,0,250,1000,-30,30,"s");
         fHistManager.CreateTH1("fh1dJetRecPt_0_95JP_bAccepted","detector level jets;pt (GeV/c); count",500,0,250,"s");
         fHistManager.CreateTH1("fh1dJetRecPt_0_95JP_cAccepted","detector level jets;pt (GeV/c); count",500,0,250,"s");
         fHistManager.CreateTH1("fh1dJetRecPt_0_95JP_udsgAccepted","detector level jets;pt (GeV/c); count",500,0,250,"s");

         //IP distributions: all jet types combined
         fHistManager.CreateTH2("fh2d_ImpSigXY_all_0_5JP", "fh2d_ImpSigXY_all_0_5JP;pt (GeV/c); sig",500,0,250,1000,-30,30,"s");
         fHistManager.CreateTH2("fh2d_ImpSigXY_all_0_6JP", "fh2d_ImpSigXY_all_0_6JP;pt (GeV/c); sig",500,0,250,1000,-30,30,"s");
         fHistManager.CreateTH2("fh2d_ImpSigXY_all_0_7JP", "fh2d_ImpSigXY_all_0_7JP;pt (GeV/c); sig",500,0,250,1000,-30,30,"s");
         fHistManager.CreateTH2("fh2d_ImpSigXY_all_0_8JP", "fh2d_ImpSigXY_all_0_8JP;pt (GeV/c); sig",500,0,250,1000,-30,30,"s");
         fHistManager.CreateTH2("fh2d_ImpSigXY_all_0_9JP", "fh2d_ImpSigXY_all_0_9JP;pt (GeV/c); sig",500,0,250,1000,-30,30,"s");
         fHistManager.CreateTH2("fh2d_ImpSigXY_all_0_95JP", "fh2d_ImpSigXY_all_0_95JP;pt (GeV/c); sig",500,0,250,1000,-30,30,"s");

         //Pt distributions: all jet types combined
         fHistManager.CreateTH1("fh1dJetRecPt_0_5JP_all_Accepted","detector level jets;pt (GeV/c); count",500,0,250,"s");
         fHistManager.CreateTH1("fh1dJetRecPt_0_6JP_all_Accepted","detector level jets;pt (GeV/c); count",500,0,250,"s");
         fHistManager.CreateTH1("fh1dJetRecPt_0_7JP_all_Accepted","detector level jets;pt (GeV/c); count",500,0,250,"s");
         fHistManager.CreateTH1("fh1dJetRecPt_0_8JP_all_Accepted","detector level jets;pt (GeV/c); count",500,0,250,"s");
         fHistManager.CreateTH1("fh1dJetRecPt_0_9JP_all_Accepted","detector level jets;pt (GeV/c); count",500,0,250,"s");
         fHistManager.CreateTH1("fh1dJetRecPt_0_95JP_all_Accepted","detector level jets;pt (GeV/c); count",500,0,250,"s");
       }//EndDoJetProb

     //MC Impact Parameter Correction
    /* fHistManager.CreateTH2("fh2dTracksImpParXY_McCorr","radial imp. parameter (after correction);impact parameter xy (cm);a.u.",2000,-1,1,500,0,100,"s");
     fHistManager.CreateTH1("fh1dTracksImpParXY_McCorr","radial imp. parameter (after correction);impact parameter xy (cm);a.u.",400,-0.2,0.2,"s");
     fHistManager.CreateTH1("fh1dTracksImpParXYZ_McCorr","3d imp. parameter (after correction);impact parameter 3d (cm);a.u.",2000,0,100.,"s");
     fHistManager.CreateTH2("fh2dTracksImpParXYZ_McCorr","XYZ imp. parameter ;impact parameter xy (cm);a.u.",2000,-1,1,500,0,100.,"s");
     fHistManager.CreateTH2("fh2dTracksImpParXYZSignificance_McCorr","XYZ imp. parameter ;impact parameter xy (cm);a.u.",2000,-30,30,500,0,100.,"s");
     fHistManager.CreateTH1("fh1dTracksImpParXYSignificance_McCorr","radial imp. parameter (after correction);impact parameter xy significance;a.u.",2000,-30,30.,"s");
     fHistManager.CreateTH1("fh1dTracksImpParXYZSignificance_McCorr","3d imp. parameter (after correction);impact parameter 3d significance;a.u.",2000,0.,100.,"s");*/

     //MC General Information
    //    }//EndPythiaLoop





    //Impact Parameter Template Generation
    const char * flavour[6]  = {"Unidentified","udsg","c","b","s",""};
    const char * base = "fh2dJetSignedImpPar";
    const char * dim[2]  = {"XY","XYZ"};
    const char * typ[2]  = {"","Significance"};
    const char * ordpar [4] = {"","First","Second","Third"};
    const char * special [1] = {"",/*"McCorr"*/};

    Int_t ptbins = 250;
    Double_t ptlow = 0;
    Double_t pthigh = 250;
    Int_t ipbins = 1000;
    Double_t iplow = -.5;
    Double_t iphigh = .5;
    for (Int_t id = 0;id<1;++id)  // XY or XY/
      for (Int_t ifl = 0;ifl<6;++ifl)  //flavour
        for (Int_t io = 0;io<4;++io)        //order parameter
          for (Int_t is = 0;is<1;++is)          //special comment
            for (Int_t it = 1;it<2;++it){           //significance or not
              if(it==1) {
                iplow=-30;
                iphigh=30; //from 30
                if(io==0 && ifl==4) ipbins = 1000;//2000;
                  else  ipbins =1000;//2000;
              }else {
                iplow=-0.5;
                iphigh=0.5;
                ipbins =1000;//;2000;
              }
              if(id==0)  ipbins =1000;//2000;
                if((fIsPythia||(!fIsPythia && ifl==5))){
                  fHistManager.CreateTH2(Form("%s%s%s%s%s%s",base,dim[id],typ[it],flavour[ifl],ordpar[io],special[is]),
                                Form("%s%s%s%s%s%s;;",base,dim[id],typ[it],flavour[ifl],ordpar[io],special[is]),
                                ptbins,ptlow,pthigh,ipbins,iplow,iphigh,"s");
                  //printf("Generating%s%s%s%s%s%s",base,dim[id],typ[it],flavour[ifl],ordpar[io],special[is]);

                  }
              }

    TIter next(fHistManager.GetListOfHistograms());
    TObject* obj = 0;
    while ((obj = next())) {
      printf("Adding Object %s\n",obj->GetName());
      fOutput->Add(obj);
    }

    PostData(1, fOutput);
}

void AliAnalysisTaskHFJetIPQA::UserExecOnce(){
    AliJetContainer *  jetconrec = static_cast<AliJetContainer*>(fJetCollArray.At(0));
    fAnalysisCuts[bAnalysisCut_MinJetPt]=jetconrec->GetJetPtCut();
    fAnalysisCuts[bAnalysisCut_MaxJetPt]=jetconrec->GetJetPtCutMax();
    fAnalysisCuts[bAnalysisCut_MinJetEta]=jetconrec->GetMinEta();
    fAnalysisCuts[bAnalysisCut_MaxJetEta]=jetconrec->GetMaxEta();

    PrintSettings();
}

void AliAnalysisTaskHFJetIPQA::PrintSettings(){
    TString jetcuts="";
    TString trackcuts="";
    TString vertexcuts="";
    Int_t version=1;


    jetcuts+=version;
    jetcuts+="+";
    jetcuts+=fAnalysisCuts[bAnalysisCut_MinJetPt];
    jetcuts+="+";
    jetcuts+=fAnalysisCuts[bAnalysisCut_MaxJetPt];
    jetcuts+="+";
    jetcuts+=fAnalysisCuts[bAnalysisCut_MinJetEta];
    jetcuts+="+";
    jetcuts+=fAnalysisCuts[bAnalysisCut_MaxJetEta];
    jetcuts+="+";
    jetcuts+=fNoJetConstituents;
    jetcuts+="+";
    jetcuts+=fDaughtersRadius;

    printf("Cut Track Settings: %s\n",jetcuts.Data());

    trackcuts+=version;
    trackcuts+="+";
    trackcuts+=Form("%0.2f",fAnalysisCuts[bAnalysisCut_DCAJetTrack]);
    trackcuts+="+";
    trackcuts+=fAnalysisCuts[bAnalysisCut_MaxDecayLength];
    trackcuts+="+";
    trackcuts+=fAnalysisCuts[bAnalysisCut_MinTrackPt];
    trackcuts+="+";
    trackcuts+=fAnalysisCuts[bAnalysisCut_MinTrackPtMC];
    trackcuts+="+";
    trackcuts+=fAnalysisCuts[bAnalysisCut_MaxDCA_Z];
    trackcuts+="+";
    trackcuts+=fAnalysisCuts[bAnalysisCut_MaxDCA_XY];
    trackcuts+="+";
    trackcuts+=fAnalysisCuts[bAnalysisCut_MinTPCClus];
    trackcuts+="+";
    trackcuts+=fAnalysisCuts[bAnalysisCut_MinITSLayersHit];
    trackcuts+="+";
    trackcuts+=fAnalysisCuts[bAnalysisCut_MinTrackChi2];
    trackcuts+="+";
    trackcuts+=fAnalysisCuts[bAnalysisCut_HasSDD];
    trackcuts+="+";
    trackcuts+=fAnalysisCuts[bAnalysisCut_KinkCand];
    trackcuts+="+";
    trackcuts+=fAnalysisCuts[bAnalysisCut_HasTPCrefit];
    trackcuts+="+";
    trackcuts+=fAnalysisCuts[bAnalysisCut_HasITSrefit];


    printf("Cut Vertex Settings %s\n", trackcuts.Data());

    vertexcuts+=version;
    vertexcuts+="+";
    vertexcuts+=Form("%0.f",fAnalysisCuts[bAnalysisCut_NContibutors]);
    vertexcuts+="+";
    vertexcuts+=Form("%0.f",fAnalysisCuts[bAnalysisCut_Sigma_Z]);
    vertexcuts+="+";
    vertexcuts+=Form("%0.f",fAnalysisCuts[bAnalysisCut_Sigma_Y]);
    vertexcuts+="+";
    vertexcuts+=Form("%0.f",fAnalysisCuts[bAnalysisCut_RelError_Z]);
    vertexcuts+="+";
    vertexcuts+=Form("%0.f",fAnalysisCuts[bAnalysisCut_RelError_Y]);
    vertexcuts+="+";
    vertexcuts+=fAnalysisCuts[bAnalysisCut_SigmaDiamond];
    vertexcuts+="+";
    vertexcuts+=fAnalysisCuts[bAnalysisCut_PtHardAndJetPtFactor];
    vertexcuts+="+";
    vertexcuts+=fAnalysisCuts[bAnalysisCut_MinNewVertexContrib];
    vertexcuts+="+";
    vertexcuts+=fAnalysisCuts[bAnalysisCut_MaxVtxZ];
    vertexcuts+="+";
    vertexcuts+=fAnalysisCuts[bAnalysisCut_Z_Chi2perNDF];
    vertexcuts+="+";
    vertexcuts+=fVertexRecalcMinPt;
    vertexcuts+="+";
    vertexcuts+=fDoMCCorrection;
    vertexcuts+="+";
    vertexcuts+=fRunSmearing;
    vertexcuts+="+";
    vertexcuts+=fDoUnderlyingEventSub;
    vertexcuts+="+";
    vertexcuts+=fDoFlavourMatching;

    fh1dCutsPrinted->SetTitle(jetcuts.Data());
    fh1dCutsPrinted->GetXaxis()->SetTitle(trackcuts.Data());
    fh1dCutsPrinted->GetYaxis()->SetTitle(vertexcuts.Data());

    printf("Vertex Cuts: %s\n",vertexcuts.Data());
}

//NotInUse
/*void AliAnalysisTaskHFJetIPQA::GetMaxImpactParameterCutR(const AliVTrack * const track, Double_t &maximpactRcut){
    //
    // Get max impact parameter cut r (pt dependent)
    //
    Double_t pt = track->Pt();
    if(pt > 0.15) {
            maximpactRcut = 0.0182 + 0.035/TMath::Power(pt,1.01);  // abs R cut
        }
        else maximpactRcut = 9999999999.0;
    }*/


    bool AliAnalysisTaskHFJetIPQA::GetPIDCombined(AliAODTrack * track, double  * prob, int &nDetectors,UInt_t &usedDet ,AliPID::EParticleType &MostProbablePID, bool setTrackPID ){
        AliPIDResponse::EDetPidStatus status[AliPIDResponse::kNdetectors] = {AliPIDResponse::kDetNoSignal,AliPIDResponse::kDetNoSignal,AliPIDResponse::kDetNoSignal,AliPIDResponse::kDetNoSignal,AliPIDResponse::kDetNoSignal,AliPIDResponse::kDetNoSignal,AliPIDResponse::kDetNoSignal};
        unsigned int nGoodDet = 0;
        for (int j =0; j<AliPIDResponse::kNdetectors;j++)
        {
            for (int i =AliPID::kElectron; i<AliPID::kSPECIES;i++)
            {
                double val = 0;
                status[j] =  fPidResponse->NumberOfSigmas(static_cast <AliPIDResponse::EDetector>(j), track, static_cast <AliPID::EParticleType>(i), val);
                if (status[j] == AliPIDResponse::kDetPidOk ){
                    nGoodDet++;}
                }
            }
            if( nGoodDet/7 <2 ) return false;
            nDetectors = nGoodDet/7;
            Double_t probTPCTOF[AliPID::kSPECIES]={-1.,-1.,-1.,-1.,-1.};
            fCombined->SetDefaultTPCPriors();
            fCombined->SetDetectorMask(AliPIDResponse::kDetITS|AliPIDResponse::kDetTPC|AliPIDResponse::kDetTOF|AliPIDResponse::kDetTRD|AliPIDResponse::kDetEMCAL);
            usedDet  = fCombined->ComputeProbabilities((AliVTrack*)track, fPidResponse   , probTPCTOF);
            int maxpid=14;
            double maxpidv=0;
            for (int j =0 ;j< AliPID::kSPECIES;++j ){
                prob[j] =probTPCTOF[j];
                if(prob[j] >maxpidv ){
                    maxpid=j;
                    maxpidv=prob[j] ;
                }
            }
            MostProbablePID=static_cast <AliPID::EParticleType>(maxpid);
            return true;
        }


        bool AliAnalysisTaskHFJetIPQA::IsFromElectron(AliAODTrack*track){
       
            Double_t p[5] ={0};
            Int_t nDet=0;
            UInt_t nDetUCom=0;
            AliPID::EParticleType mpPID=AliPID::kUnknown;
            if(GetPIDCombined( track, p, nDet,nDetUCom ,mpPID, kFALSE )){
                if(mpPID==AliPID::kElectron && p[0] >0.90 && nDet >1) return kTRUE;
            }
            return false;
        }

        bool AliAnalysisTaskHFJetIPQA::IsFromPion(AliAODTrack*track){
            Double_t p[5] ={0};
            Int_t nDet=0;
            UInt_t nDetUCom=0;
            AliPID::EParticleType mpPID=AliPID::kUnknown;

            if(GetPIDCombined( track, p, nDet,nDetUCom ,mpPID, kFALSE )){
                if(mpPID==AliPID::kPion && p[2] >0.90 && nDet >1) return kTRUE;
            }
            return false;
        }
        bool AliAnalysisTaskHFJetIPQA::IsFromKaon(AliAODTrack*track){
            Double_t p[5] ={0};
            Int_t nDet=0;
            UInt_t nDetUCom=0;
            AliPID::EParticleType mpPID=AliPID::kUnknown;
            if(GetPIDCombined( track, p, nDet,nDetUCom ,mpPID, kFALSE )){
                if(mpPID==AliPID::kKaon && p[3] >0.90 && nDet >1) return kTRUE;
            }
            return false;
        }
        bool AliAnalysisTaskHFJetIPQA::IsFromProton(AliAODTrack*track){
            Double_t p[5] ={0};
            Int_t nDet=0;
            UInt_t nDetUCom=0;
            AliPID::EParticleType mpPID=AliPID::kUnknown;
            if(GetPIDCombined( track, p, nDet,nDetUCom ,mpPID, kFALSE )){
                if(mpPID==AliPID::kProton && p[3] >0.90 && nDet >1) return kTRUE;
            }
            return false;
        }

        int AliAnalysisTaskHFJetIPQA::DetermineUnsuitableVtxTracks(int *skipped, AliAODEvent * const aod, AliVTrack * const track){
            Int_t nTracks=aod->GetNumberOfTracks();
            AliAODTrack * t = nullptr;
            AliExternalTrackParam etp_at_r39_old; etp_at_r39_old.CopyFromVTrack(track);
            etp_at_r39_old.PropagateTo(3.9,InputEvent()->GetMagneticField());
            double angle0 = TMath::ATan2(etp_at_r39_old.Yv(),etp_at_r39_old.Xv());
            double zz0    = etp_at_r39_old.GetZ();
            int nTrksToSkip=1;

            for(Int_t i=0; i<nTracks; i++){
                t = (AliAODTrack *)(aod->GetTrack(i));
                if(!((((AliAODTrack*)t)->TestFilterBit(4))))continue;
                int id = (Int_t)t->GetID();
                AliExternalTrackParam etp_at_r39; etp_at_r39.CopyFromVTrack(t);
                etp_at_r39.PropagateTo(3.9,InputEvent()->GetMagneticField());
                double angle = TMath::ATan2(etp_at_r39.Yv(),etp_at_r39.Xv());
                double zz    = etp_at_r39.GetZ();
                bool doskip=false;
                if(t->Pt()<fVertexRecalcMinPt)                      doskip=true;
                if(fabs(TVector2::Phi_mpi_pi(angle-angle0))>TMath::Pi()/6.) {
                    doskip=true;
                }
                if(fabs(zz-zz0)>0.5) {
                    doskip=true;
                }
                if(doskip && !fGlobalVertex){
                    skipped[nTrksToSkip++] = id;
                }
            }
            return nTrksToSkip;
        }

AliAODVertex *AliAnalysisTaskHFJetIPQA::RemoveDaughtersFromPrimaryVtx( const AliVTrack * const track) {
   //Initialisation of vertexer
   const AliAODEvent * aod =  ((AliAODEvent*)InputEvent());
   AliAODVertex *vtxAOD =aod ->GetPrimaryVertex();
   if(!vtxAOD) return 0;
   //printf("Before remove:\n");
   //UvtxAOD->Print();

   TString title=vtxAOD->GetTitle();
   if(!title.Contains("VertexerTracks")) return 0;

   AliVertexerTracks vertexer(aod->GetMagneticField());
   vertexer.SetITSMode();
   vertexer.SetMinClusters(3);
   if(title.Contains("WithConstraint")) {
     Float_t diamondcovxy[3];
     aod->GetDiamondCovXY(diamondcovxy);
     Double_t pos[3]={aod->GetDiamondX(),aod->GetDiamondY(),0.};
     Double_t cov[6]={diamondcovxy[0],diamondcovxy[1],diamondcovxy[2],0.,0.,10.*10.};
     AliESDVertex diamond(pos,cov,1.,1);
     vertexer.SetVtxStart(&diamond);
   }

   //_____________________________
   //Determination of unsuited tracks
   Int_t skipped[5000]; for(Int_t i=0;i<5000;i++) skipped[i]=-1;
   Int_t id = (Int_t)track->GetID();
   if(!(id<0)) skipped[0] = id;  //remove track under investigation from vertex
   int nTrksToSkip=1;
   //nTrksToSkip=DetermineUnsuitableVtxTracks(skipped, aod, track);
   vertexer.SetSkipTracks(nTrksToSkip,skipped);

   //________________________________
   //Determination of new ESD vertex
   AliESDVertex *vtxESDNew = vertexer.FindPrimaryVertex(aod);
   if(!vtxESDNew) return 0;
   Int_t nContrib =vtxESDNew->GetNContributors();
   if(vtxESDNew->GetNContributors()<fAnalysisCuts[bAnalysisCut_MinNewVertexContrib]) {
     //printf("Thrown away with %i contributors\n",vtxESDNew->GetNContributors());
     delete vtxESDNew; vtxESDNew=nullptr;
     return 0;
   }
   if(vtxESDNew->GetChi2toNDF()>fAnalysisCuts[bAnalysisCut_Z_Chi2perNDF]) {
     //printf("Thrown away with chi2 = %f\n",fAnalysisCuts[bAnalysisCut_Z_Chi2perNDF]);
     delete vtxESDNew; vtxESDNew=nullptr;
     return 0;
   }

   //________________________________
   //Conversion to AOD vertex
   Double_t pos[3];
   Double_t cov[6];
   Double_t chi2perNDF;
   vtxESDNew->GetXYZ(pos); // position
   vtxESDNew->GetCovMatrix(cov); //covariance matrix
   chi2perNDF = vtxESDNew->GetChi2toNDF(); //chisquare
   if(vtxESDNew) delete vtxESDNew;
   vtxESDNew=NULL;
   AliAODVertex *vtxAODNew = new AliAODVertex(pos,cov,chi2perNDF);
   vtxAODNew->SetNContributors(nContrib);  //contributors
   return vtxAODNew;
}
void AliAnalysisTaskHFJetIPQA::FillParticleCompositionSpectra(AliEmcalJet * jet,const char * histname ){
    if(!jet) return;
    AliVTrack* tr=0x0; 
    for(Int_t j = 0; j < jet->GetNumberOfTracks(); ++j) {
      tr = (AliVTrack*)GetParticleContainer(0)->GetParticle((jet->TrackAt(j)));
      if(!tr) continue;
      double pT=0x0;
      Int_t pCorr_indx=-1;
      GetMonteCarloCorrectionFactor(tr,pCorr_indx,pT);
      if(pCorr_indx<0 ) continue;
      FillHist(histname,pCorr_indx+0.5,pT, 1);     //*this->fXsectionWeightingFactor );
  }
  return;
}

/*! \brief FillParticleCompositionEvent
 *
 *
 * Fill Histogram with Correction Factors vs. pT
 */
void AliAnalysisTaskHFJetIPQA::FillParticleCompositionEvent( ){
    AliVTrack* tr=0x0; 
    for(Int_t j = 0; j < InputEvent()->GetNumberOfTracks(); ++j) {
      tr = (AliVTrack*)(InputEvent()->GetTrack(j));
      if(!tr) continue;
      double pT=0x0;
      Int_t pCorr_indx=-1;
      GetMonteCarloCorrectionFactor(tr,pCorr_indx,pT);
      if(pCorr_indx<0) continue;
      FillHist("fh2dParticleSpectra_Event",pCorr_indx+0.5,pT, 1);     //*this->fXsectionWeightingFactor );
  }
  return;
}


void AliAnalysisTaskHFJetIPQA::GetOutOfJetParticleComposition(AliEmcalJet * jet, int flavour){
    if(!jet) return;
    AliEmcalJet * perp_jet1 =   GetPerpendicularPseudoJet(jet,false);
    AliEmcalJet * perp_jet2 =   GetPerpendicularPseudoJet(jet,true);
    FillParticleCompositionSpectra(jet,"fh2dParticleSpectra_InCone");
    if(flavour==3)  FillParticleCompositionSpectra(jet,"fh2dParticleSpectra_InCone_bjet");
    else if(flavour==2)  FillParticleCompositionSpectra(jet,"fh2dParticleSpectra_InCone_cjet");
    else if(flavour==1)  FillParticleCompositionSpectra(jet,"fh2dParticleSpectra_InCone_lfjet");
    FillParticleCompositionSpectra(perp_jet1,"fh2dParticleSpectra_OutOfCone");
    FillParticleCompositionSpectra(perp_jet2,"fh2dParticleSpectra_OutOfCone");
    if(perp_jet1) delete perp_jet1;
    if(perp_jet2) delete perp_jet2;
}

AliEmcalJet *  AliAnalysisTaskHFJetIPQA::GetPerpendicularPseudoJet (AliEmcalJet *jet_in  , bool rev ){
    TVector3 j(jet_in->Px(), jet_in->Py(), jet_in->Pz());
    TVector3 p1(j);
    std::vector <int > track_inc;
    p1.RotateZ(rev ? -1*TMath::Pi()/2. :TMath::Pi()/2. );
    Double_t sumAllPt1 = 0;
    int nconst_1 =0;
    std::vector <int> const_idx1;   
    for(long itrack= 0; itrack<GetParticleContainer(0)->GetNParticles();++itrack){
       AliVTrack *  tr = static_cast<AliVTrack*>(GetParticleContainer(0)->GetParticle(itrack));
       if(!tr) continue;
       TVector3 v(tr->Px(), tr->Py(), tr->Pz());
       Double_t dR1 = v.DrEtaPhi(p1);
       if(v.Pt()>0.150){
        if(dR1 < fJetRadius) {
            sumAllPt1+=v.Pt();
            nconst_1++;
            const_idx1.push_back(itrack);
        }
    } 
}
AliEmcalJet* jet1 =0;


if (sumAllPt1>0) {
    jet1 = new AliEmcalJet(sumAllPt1, p1.Eta(), TVector2::Phi_0_2pi (p1.Phi()), 0);
    jet1->SetArea(fJetRadius*fJetRadius*TMath::Pi());
    jet1->SetNumberOfTracks(nconst_1);
    jet1->SetNumberOfClusters(0);
    for (int i = 0 ; i < (int) const_idx1.size();++i) {
        jet1->AddTrackAt(const_idx1.at(i), i);
    }}


    return jet1;
}










/*! \brief CalculateTrackImpactParameter
 *
 *
 * Calculate track impact parameter
 */

Double_t AliAnalysisTaskHFJetIPQA::GetValImpactParameter(TTypeImpPar type,Double_t *impar, Double_t * cov)
{
    Float_t result = -999999;
    Float_t dFdx   = 0;
    Float_t dFdy   = 0;
    switch(type){
        case kXY:
        result = impar[0];
        break;
        case kXYSig:
        result =  impar[0]/sqrt(cov[0]);
        break;
        case kXYZ:
        result = TMath::Sqrt(impar[0]*impar[0]+impar[1]*impar[1]);
        break;
        case kXYZSig:
        result =  TMath::Sqrt(impar[0]*impar[0]+impar[1]*impar[1]);
        dFdx = impar[0]/result;
        dFdy = impar[1]/result;
        result /=TMath::Sqrt(cov[0]*dFdx*dFdx + cov[2]*dFdy*dFdy + 2* cov[1] *dFdx*dFdy);
        break;
        case kZSig:
        result =  impar[1];
        result /=TMath::Sqrt(cov[2]);
        break;
        case kXYZSigmaOnly:
        result =  TMath::Sqrt(impar[0]*impar[0]+impar[1]*impar[1]);
        dFdx = impar[0]/result;
        dFdy = impar[1]/result;
        result =TMath::Sqrt(cov[0]*dFdx*dFdx + cov[2]*dFdy*dFdy + 2* cov[1] *dFdx*dFdy);
        break;
        default:
        break;
    }
    return result;
}

//____________________________________________________
void AliAnalysisTaskHFJetIPQA::FillCandidateJet(Int_t CutIndex, Int_t JetFlavor){
        if(JetFlavor>0) fh1DCutInclusive->Fill(CutIndex);
        if(fIsPythia){
                if(JetFlavor==1)fh1dCutudg->Fill(CutIndex);
                if(JetFlavor==2)fh1dCutc->Fill(CutIndex);
                if(JetFlavor==3)fh1dCutb->Fill(CutIndex);
                if(JetFlavor==4)fh1dCutb->Fill(CutIndex);
        }

}

Bool_t AliAnalysisTaskHFJetIPQA::IsTrackAccepted(AliVTrack* track , int jetflavour){
    if(!track) return kFALSE;
    if(fIsEsd){
        fESDTrackCut->SetMinNClustersITS(fAnalysisCuts[bAnalysisCut_MinITSLayersHit]);
        fESDTrackCut->SetClusterRequirementITS(AliESDtrackCuts::kSPD,AliESDtrackCuts::kAny);
        if(!(fESDTrackCut->AcceptTrack((AliESDtrack*)track))) return kFALSE;
        return kTRUE;
    }
    else {
            //HasMatchedGoodTracklet((AliAODTrack*)track);
        if(!(((AliAODTrack*)track)->TestFilterBit(9) || ((AliAODTrack*)track)->TestFilterBit(4)))return kFALSE;

        if(TMath::Abs(track->Eta())>0.9) return kFALSE;

        //SPD hits
        if(fAnalysisCuts[bAnalysisCut_HasSDD]){
          if(!(((AliAODTrack*)track->HasPointOnITSLayer(0))&&(AliAODTrack*)track->HasPointOnITSLayer(1))){
            FillCandidateJet(bAnalysisCut_HasSDD,jetflavour);
            return kFALSE;
          }
        }

        //n=hits in ITS layers
        Int_t SPDSSDHits = (int) track->HasPointOnITSLayer(0) + (int) track->HasPointOnITSLayer(1) + (int) track->HasPointOnITSLayer(4) + (int) track->HasPointOnITSLayer(5);
        if(SPDSSDHits<abs(fAnalysisCuts[bAnalysisCut_MinITSLayersHit])){
            //printf("Throw away due flav=%i ssd points 0 = %i, 1=%i, 2=%i, 3=%i, SPDSSDHits=%i cutvalue=%f\n",jetflavour,track->HasPointOnITSLayer(0),track->HasPointOnITSLayer(1),track->HasPointOnITSLayer(4),track->HasPointOnITSLayer(5),SPDSSDHits, abs(fAnalysisCuts[bAnalysisCut_MinITSLayersHit]));
            FillCandidateJet(bAnalysisCut_MinITSLayersHit,jetflavour);
            return kFALSE;
        }

        //TPC clusters
        if(((AliAODTrack*)track)->GetNcls(1)<fAnalysisCuts[bAnalysisCut_MinTPCClus]){
            //printf("Throw away due flav=%i TPCClus %i, cutvalue=%f\n",jetflavour,((AliAODTrack*)track)->GetNcls(1),fAnalysisCuts[bAnalysisCut_MinTPCClus]);
            FillCandidateJet(bAnalysisCut_MinTPCClus,jetflavour);
            return kFALSE;
        }

        if(track->Pt()<fAnalysisCuts[bAnalysisCut_MinTrackPt]){
            //printf("Throw away due flav=%i pt %f, cutvalue=%f\n",jetflavour,track->Pt(),fAnalysisCuts[bAnalysisCut_MinTrackPt]);
            FillCandidateJet(bAnalysisCut_MinTrackPt,jetflavour);
            return kFALSE;
        }

        AliAODVertex *aodvertex = (( AliAODTrack *)track)->GetProdVertex();
        if(!aodvertex) return kFALSE;
        if(fAnalysisCuts[bAnalysisCut_KinkCand]){
          if(aodvertex->GetType()==AliAODVertex::kKink){
            FillCandidateJet(bAnalysisCut_KinkCand,jetflavour);
            return kFALSE;
          }
        }

        ULong_t status=track->GetStatus();
        if(fAnalysisCuts[bAnalysisCut_HasTPCrefit]){
          if(!(status & AliAODTrack::kTPCrefit)){
            FillCandidateJet(bAnalysisCut_HasTPCrefit,jetflavour);
            return kFALSE;
          }
        }

        if(fAnalysisCuts[bAnalysisCut_HasITSrefit]){
          if(!(status & AliAODTrack::kITSrefit)){
            FillCandidateJet(bAnalysisCut_HasITSrefit,jetflavour);
            return kFALSE;
          }
        }
        if(((AliAODTrack*)track)->Chi2perNDF()>=fAnalysisCuts[bAnalysisCut_MinTrackChi2]){
            FillCandidateJet(bAnalysisCut_MinTrackChi2,jetflavour);
            //printf("Throw away due flav=%i chi2 %f, cutvalue=%f\n",jetflavour,((AliAODTrack*)track)->Chi2perNDF(),fAnalysisCuts[bAnalysisCut_MinTrackChi2]);
            return kFALSE;
        }

        return kTRUE;
    }
    return kTRUE;
}


bool AliAnalysisTaskHFJetIPQA::IsDCAAccepted(double decaylength, double ipwrtjet, Double_t * dca, int jetflavour){
    if(dca[0]>fAnalysisCuts[bAnalysisCut_MaxDCA_XY]){
        FillCandidateJet(bAnalysisCut_MaxDCA_XY,jetflavour);
        //printf("Throw away due to xy cut = %f, cutvalue=%f",dca[0],fAnalysisCuts[bAnalysisCut_MaxDCA_XY]);
        return kFALSE;
    }
    if(dca[1]>fAnalysisCuts[bAnalysisCut_MaxDCA_Z]){
        FillCandidateJet(bAnalysisCut_MaxDCA_Z,jetflavour);
        printf("Throw away due to z cut = %f, cutvalue=%f",dca[1],fAnalysisCuts[bAnalysisCut_MaxDCA_Z]);
        return kFALSE;
    }
    if(decaylength>fAnalysisCuts[bAnalysisCut_MaxDecayLength]){
        FillCandidateJet(bAnalysisCut_MaxDecayLength,jetflavour);
        //printf("Throw away due to decaylength cut = %f, cutvalue=%f",decaylength,fAnalysisCuts[bAnalysisCut_MaxDecayLength]);
        return kFALSE;
    }
    if(ipwrtjet>fAnalysisCuts[bAnalysisCut_DCAJetTrack]){
        FillCandidateJet(bAnalysisCut_DCAJetTrack,jetflavour);
        //printf("Throw away due to dcajettrack = %f, cutvalue=%f",ipwrtjet,fAnalysisCuts[bAnalysisCut_DCAJetTrack]);
        return kFALSE;
    }

    return kTRUE;
}

/*! \brief FillTrackingEfficiencyDCA
 *
 *
 * Calculates tracking efficiency as a function of the dca xy z
 */

/*! \brief MatchJetsGeometricDefault
 *
 *
 * geometric jet matching
 */
Bool_t AliAnalysisTaskHFJetIPQA::MatchJetsGeometricDefault()
{
    AliJetContainer *jets1 = static_cast<AliJetContainer*>(fJetCollArray.At(0));
    AliJetContainer *jets2 = static_cast<AliJetContainer*>(fJetCollArray.At(1));
    Double_t matchingpar1 =0.25;
    Double_t matchingpar2 =0.25;
    if (!jets1 || !jets1->GetArray() || !jets2 || !jets2->GetArray()) return kFALSE;
    DoJetLoop();
    AliEmcalJet* jet1 = 0;
    jets1->ResetCurrentID();
    while ((jet1 = jets1->GetNextJet())) {
        AliEmcalJet *jet2 = jet1->ClosestJet();
        if (!jet2) continue;
        if (jet2->ClosestJet() != jet1) continue;
        if (jet1->ClosestJetDistance() > matchingpar1 || jet2->ClosestJetDistance() > matchingpar2) continue;
            // Matched jet found
        jet1->SetMatchedToClosest(1);
        jet2->SetMatchedToClosest(1);
    }
    return kTRUE;
}

/*! \brief DoJetLoop
 *
 *
 * jet matching loop:
 * - reset previous matching and jet IDs
 * - redo jet matching
 */
void AliAnalysisTaskHFJetIPQA::DoJetLoop()
{
    // Do the jet loop.
    Double_t minjetpt =1.;
    AliJetContainer *jets1 = static_cast<AliJetContainer*>(fJetCollArray.At(0));
    AliJetContainer *jets2 = static_cast<AliJetContainer*>(fJetCollArray.At(1));
    if (!jets1 || !jets1->GetArray() || !jets2 || !jets2->GetArray()) return;
    AliEmcalJet* jet1 = 0;
    AliEmcalJet* jet2 = 0;
    jets2->ResetCurrentID();
    while ((jet2 = jets2->GetNextJet())) jet2->ResetMatching();
    jets1->ResetCurrentID();
    while ((jet1 = jets1->GetNextJet())) {
        jet1->ResetMatching();
        if (jet1->MCPt() < minjetpt) continue;
        jets2->ResetCurrentID();
        while ((jet2 = jets2->GetNextJet())) {
            SetMatchingLevel(jet1, jet2, 1);
                } // jet2 loop
        } // jet1 loop
    }
/*! \brief IsTruePrimary
 *
 *
 *
 */
    Bool_t AliAnalysisTaskHFJetIPQA::IsTruePrimary(AliVParticle * mcpart){
        if(!mcpart) return kFALSE;
        AliVParticle * mcmother = GetVParticleMother(mcpart);
        if(!mcmother) return kTRUE;
        Int_t istatus =-1;
        if(!fIsEsd) {
            istatus =   ( (AliMCParticle*)mcmother)->Particle()->GetStatusCode();
        }
        else {
            istatus =    ((AliAODMCParticle*)mcmother)->GetStatus();
        }
        if(istatus >11)return kTRUE;
        return kFALSE;
    }
/*! \brief Composition correction factor  getter
 *
 * finds the corresponding re-weighing factor for a certain track:
 *      - find mother particle
 *      - define fluca factor according to mother particle
 */
    Double_t AliAnalysisTaskHFJetIPQA::GetWeightFactor( AliVTrack * track,Int_t &pCorr_indx, double &ppt){
        AliMCParticle *pMCESD = nullptr;
        AliAODMCParticle *pMCAOD = nullptr;
        if(track->GetLabel()< 0) return 1;
        if(!fIsPythia) return 1;
        if(fIsEsd){
            pMCESD = ((AliMCParticle*)MCEvent()->GetTrack(abs(track->GetLabel())));
            if(!(pMCESD)) return 1;
        }
        else {
            pMCAOD = static_cast<AliAODMCParticle*>(fMCArray->At(abs(track->GetLabel())));
            if(!(pMCAOD)) return 1;
        }

        AliVParticle * mcpart = fIsEsd ? (   AliVParticle * )pMCESD:(   AliVParticle * )pMCAOD;
        Bool_t _particlesourcefound(kFALSE);
        Int_t  _particlesourcepdg(mcpart->PdgCode());
        Int_t  _particlesourceidx(25);
        Double_t _particlesourcept(0);

        AliVParticle * mcpartclone = mcpart;
    while(mcpart){//Strangenss
        if((abs(mcpart->PdgCode()) >0 && abs(mcpart->PdgCode()) <7)|| (abs(mcpart->PdgCode())  == 21)) break;
        _particlesourcept = mcpart->Pt();
        _particlesourcepdg = abs(mcpart->PdgCode());
        if (IsSelectionParticleStrange(mcpart,_particlesourcepdg,_particlesourcept,_particlesourceidx)){
            _particlesourcefound = kTRUE;
            break;
        }
        mcpart = GetVParticleMother(mcpart);
    }
    if (!_particlesourcefound) { //heavy mesons to improve templates
        mcpart = mcpartclone;

        while(mcpart){
            if((abs(mcpart->PdgCode()) >0 && abs(mcpart->PdgCode()) <7)|| (abs(mcpart->PdgCode())  == 21)) break;
            _particlesourcept = mcpart->Pt();
            _particlesourcepdg = abs(mcpart->PdgCode());
            if (IsSelectionParticleMeson(mcpart,_particlesourcepdg,_particlesourcept,_particlesourceidx)){
                _particlesourcefound = kTRUE;
                break;
            }
            mcpart = GetVParticleMother(mcpart);
        }
    }
    if (!_particlesourcefound) { //charged hadrons
        mcpart = mcpartclone;
        while(mcpart){
            if((abs(mcpart->PdgCode()) >0 && abs(mcpart->PdgCode()) <7)|| (abs(mcpart->PdgCode())  == 21)) break;
            _particlesourcept = mcpart->Pt();
            _particlesourcepdg = abs(mcpart->PdgCode());
            if (IsSelectionParticleOmegaXiSigmaP(mcpart,_particlesourcepdg,_particlesourcept,_particlesourceidx)){
                _particlesourcefound = kTRUE;
                break;
            }
            mcpart = GetVParticleMother(mcpart);
        }
    }
    if (!_particlesourcefound) {
        mcpart = mcpartclone;
        while(mcpart){
            if((abs(mcpart->PdgCode()) >0 && abs(mcpart->PdgCode()) <7)|| (abs(mcpart->PdgCode())  == 21)) break;
            _particlesourcept = mcpart->Pt();
            _particlesourcepdg = abs(mcpart->PdgCode());
            if (IsSelectionParticle(mcpart,_particlesourcepdg,_particlesourcept,_particlesourceidx)){
                _particlesourcefound = kTRUE;
                break;
            }
            mcpart = GetVParticleMother(mcpart);
        }
    }
    if (!_particlesourcefound) {
        mcpart = mcpartclone;
        while(mcpart){
            if((abs(mcpart->PdgCode()) >0 && abs(mcpart->PdgCode()) <7)|| (abs(mcpart->PdgCode())  == 21)) break;
            _particlesourcept = mcpart->Pt();
            if (IsSelectionParticleALICE(mcpart,_particlesourcepdg,_particlesourcept,_particlesourceidx)){
                _particlesourcefound = kTRUE;
                break;
            }
            mcpart = GetVParticleMother(mcpart);
        }
    }

    if (!_particlesourcefound) return 1.;
    // Do the weighting
    Double_t factor = 1;
    if (_particlesourceidx <0) return 1;
    if (_particlesourceidx >19) return 1;
    Double_t wpos = ((_particlesourcept - 0.15)/ 0.05);
    Double_t  fractpart, intpart;
    fractpart = modf (wpos , &intpart);
    if (fractpart > 0) intpart = intpart + 1;
    Int_t  bin = floor(intpart);
    if (bin > 497) bin = 497;// above weight definition
    if (_particlesourcept < 0.1+ 1E-5) bin = 0; //below weight definition
    if((_particlesourceidx == bIdxSigmaMinus) || (_particlesourceidx == bIdxSigmaPlus))    factor = fBackgroundFactorLinus[bIdxLambda][bin];
    else factor = fBackgroundFactorLinus[_particlesourceidx][bin];
    pCorr_indx = _particlesourceidx;
    Double_t flucafactor = 1;

    switch(mcpart->PdgCode())
    {
        case -bPhi:
        factor=1;
        flucafactor =1./fPhi->Eval(_particlesourcept);
        pCorr_indx=bIdxPhi;
        break;
        case -bK0S892:
        factor=1;
        flucafactor =1./fK0Star->Eval(_particlesourcept);
        pCorr_indx=bIdxK0S892;
        break;
        case -bK0S892plus:
        factor=1;
        flucafactor =1./fK0Star->Eval(_particlesourcept);
        pCorr_indx=bK0S892plus;
        break;
        case -bOmegaBaryon:
        pCorr_indx=bIdxOmegaBaryon;
        factor=1;
        flucafactor =1./fGraphOmega->Eval(_particlesourcept);
        break;
        case -bXiBaryon:
        pCorr_indx=bIdxXiBaryon;
        factor=1;
        flucafactor =1./fGraphXi->Eval(_particlesourcept);
        break;
        case bPhi:
        factor=1;
        flucafactor =1./fPhi->Eval(_particlesourcept);
        pCorr_indx=bIdxPhi;
        break;
        case bK0S892:
        factor=1;
        pCorr_indx=bIdxK0S892;
        flucafactor =1./fK0Star->Eval(_particlesourcept);
        break;
        case bK0S892plus:
        pCorr_indx=bK0S892plus;
        factor=1;
        flucafactor =1./fK0Star->Eval(_particlesourcept);
        break;
        case bOmegaBaryon:
        pCorr_indx=bIdxOmegaBaryon;
        factor=1;
        flucafactor =fGraphOmega->Eval(_particlesourcept);
        break;
        case bXiBaryon:
        pCorr_indx=bIdxXiBaryon;
        factor=1;
        flucafactor =fGraphXi->Eval(_particlesourcept);
        break;

        default:
        break;
    }
    factor*=flucafactor;
    if (factor <= 0 || factor > 100.)  {
        return 1;
    }
    ppt = _particlesourcept;
    return factor ;
}

/*! \brief GetBMesonWeight
 *
 *
 */
Bool_t AliAnalysisTaskHFJetIPQA::GetBMesonWeight( AliVParticle * mcpart ,Int_t &pdg,Double_t &pT,Int_t &idx  )
{
    pT = mcpart->Pt();
    switch(pdg){
        case bBPlus:
        idx = bIdxBPlus;
        return kTRUE;
        case bB0:
        idx = bIdxB0;
        return kTRUE;
        case bLambdaB:
        idx = bIdxLambdaB;
        return kTRUE;
        break;
        case bBStarPlus:
        idx = bIdxBStarPlus;
        return kTRUE;
        break;
        default:
        break;
    }
    return kFALSE;
}
/*! \brief IsSelectionParticle
 *
 *
 */
Bool_t AliAnalysisTaskHFJetIPQA::IsSelectionParticle( AliVParticle *  mcpart ,Int_t &pdg,Double_t &pT,Int_t &idx ){
    pT 	= mcpart->Pt();
    Int_t pdg2 = abs(mcpart->PdgCode());
    idx = -1;

    switch(pdg2){
        case bPi0:
        idx = bIdxPi0;
        if(!IsSecondaryFromWeakDecay(mcpart))return kTRUE;
        break;
        case bEta:
        idx = bIdxEta;
        if(!IsSecondaryFromWeakDecay(mcpart))return kTRUE;
        break;
        case bEtaPrime:
        idx = bIdxEtaPrime;
        if(!IsSecondaryFromWeakDecay(mcpart))return kTRUE;
        break;
        case bOmega:
        idx = bIdxOmega;
        if(!IsSecondaryFromWeakDecay(mcpart))return kTRUE;
        break;
        case bPhi:
        idx = bIdxPhi;
        if(!IsSecondaryFromWeakDecay(mcpart))return kTRUE;
        break;
        case bRho:
        idx = bIdxRho;
        if(!IsSecondaryFromWeakDecay(mcpart))return kTRUE;
        break;
        case bRhoPlus:
        idx = bIdxRho;
        if(!IsSecondaryFromWeakDecay(mcpart))return kTRUE;
        break;
        default:
        break;
    }
    return kFALSE;
}
/*! \brief IsSelectionParticleALICE
 *
 *
 */
Bool_t AliAnalysisTaskHFJetIPQA::IsSelectionParticleALICE( AliVParticle *  mcpart ,Int_t &pdg,Double_t &pT,Int_t &idx ){
    pT 	= mcpart->Pt();
    AliVParticle * mother = nullptr;
    idx = -1;
    pdg = abs(mcpart->PdgCode());
    Bool_t pIsSecStrANGE= IsSecondaryFromWeakDecay(mcpart);
    if(pIsSecStrANGE ) return kFALSE;
    if(!IsPhysicalPrimary(mcpart))return kFALSE;

    switch(pdg){
        case bProton:
        mother = GetVParticleMother(mcpart);
        if(mother){
            if((abs(mother->PdgCode()) ==  3222) )
                return kFALSE;
        }
        idx=bIdxProton;
        return kTRUE;
        break;
        case bPi:
        idx=bIdxPi;
        return kTRUE;
        break;
        case bKaon:
        idx=bIdxKaon;
        return kTRUE;
        break;
        default:
        return kFALSE;
        break;
    }
    return kFALSE;
}
/*! \brief IsSelectionParticleMeson
 *
 *
 */
Bool_t AliAnalysisTaskHFJetIPQA::IsSelectionParticleMeson( AliVParticle *  mcpart ,Int_t &pdg,Double_t &pT,Int_t &idx ){
    pT 	= mcpart->Pt();
    idx = -1;
    pdg = abs(mcpart->PdgCode());
    if(TMath::Abs(mcpart->Y()) >=.5) return kFALSE;
    if(IsSecondaryFromWeakDecay(mcpart))return kFALSE;
    switch(pdg){
        case bD0:
        idx = bIdxD0;
        if(IsPromptDMeson(mcpart))return kTRUE;
        break;
        case bDPlus:
        idx = bIdxDPlus;
        if(IsPromptDMeson(mcpart))return kTRUE;
        break;
        case bDSPlus:
        idx = bIdxDSPlus;
        if(IsPromptDMeson(mcpart))return kTRUE;
        break;
        case bDStarPlus:
        idx = bIdxDStarPlus;
        if(IsPromptDMeson(mcpart))return kTRUE;
        break;
        case bLambdaC:
        idx = bIdxLambdaC;
        if(IsPromptDMeson(mcpart))return kTRUE;
        break;
        case bBPlus:
        idx = bIdxBPlus;
        if(IsPromptBMeson(mcpart))return kTRUE;
        break;
        case bB0:
        idx = bIdxB0;
        if(IsPromptBMeson(mcpart))return kTRUE;
        break;
        case bLambdaB:
        idx = bIdxLambdaB;
        if(IsPromptBMeson(mcpart))return kTRUE;
        break;
        case bBStarPlus:
        idx = bIdxBStarPlus;
        if(IsPromptBMeson(mcpart))return kTRUE;
        break;
        default:
        return kFALSE;
        break;
    }
    return kTRUE;
}
/*! \brief IsSelectionParticleOmegaXiSigmaP
 *
 *
 */
Bool_t AliAnalysisTaskHFJetIPQA::IsSelectionParticleOmegaXiSigmaP( AliVParticle *  mcpart ,Int_t &pdg,Double_t &pT,Int_t &idx ){
    pT 	= mcpart->Pt();
    idx = -1;
    pdg = abs(mcpart->PdgCode());

    if (!IsPhysicalPrimary(mcpart)) return kFALSE;
    switch(pdg){
        case bSigmaMinus:
            idx = bIdxSigmaMinus; // use lambda as proxy
            return kTRUE;
            break;
            case bSigmaPlus:
            idx = bIdxSigmaPlus; //use lambda as proxy
            return kTRUE;
            break;
            case bXiBaryon:
            idx = bIdxBStarPlus;//dummy! Externally sotlved
            return kTRUE;
            break;
            case bOmegaBaryon:
            idx = bIdxBStarPlus;//dummy! Externally solved
            return kTRUE;
            break;
            default:
            return kFALSE;
            break;
        }
        return kFALSE;
    }
/*! \brief GetVParticleMother
 *
 *
 */
    AliVParticle * AliAnalysisTaskHFJetIPQA::GetVParticleMother(AliVParticle * part){
        AliVParticle * mother = nullptr;
        if (part->GetMother()<0) return nullptr;
        if(fIsEsd){
            mother = static_cast <AliMCParticle*>(MCEvent()->GetTrack(part->GetMother()));
        }
        else{
            mother =static_cast<AliAODMCParticle*>(fMCArray->At(part->GetMother()));
        }
        if(!mother) return nullptr;
        return mother;
    }
/*! \brief IsPhysicalPrimary
 *
 *
 */
    Bool_t  AliAnalysisTaskHFJetIPQA::IsPhysicalPrimary( AliVParticle * part){
        return  fIsEsd ? MCEvent()->IsPhysicalPrimary(part->GetLabel()) : static_cast<AliAODMCParticle*>(part)->IsPhysicalPrimary() ;
    }
/*! \brief IsSecondaryFromWeakDecay
 *
 *
 */
    Bool_t  AliAnalysisTaskHFJetIPQA::IsSecondaryFromWeakDecay( AliVParticle * part){
        return fIsEsd ? MCEvent()->IsSecondaryFromWeakDecay(part->GetLabel()) : static_cast<AliAODMCParticle*>(part)->IsSecondaryFromWeakDecay() ;

    }
/*! \brief IsSelectionParticleStrange
 *
 *
 */
    Bool_t AliAnalysisTaskHFJetIPQA::IsSelectionParticleStrange( AliVParticle *  mcpart ,Int_t &pdg,Double_t &pT,Int_t &idx ){
        pT 	= mcpart->Pt();
        AliVParticle * mother = nullptr;
        idx = -1;
        pdg = abs(mcpart->PdgCode());
        if(!IsPhysicalPrimary(mcpart))return kFALSE;
        switch(pdg){
            case bPhi:
            idx = bIdxPhi;//dummy will be overwritten in posrpos
            return kTRUE;
            break;
            case bK0S892:
            idx = bIdxK0s;//dummy will be overwritten in posrpos
            return kTRUE;
            break;
            case bK0S892plus:
            idx = bIdxK0s; //dummy will be overwritten in posrpos
            return kTRUE;
            break;
            case bK0s:
            idx = bIdxK0s;
            mother = GetVParticleMother(mcpart);
            if(mother){
                if((abs(mother->PdgCode()) == bPhi))
                    return kFALSE;
            }
            return kTRUE;
            break;
            case bK0l:
            idx = bIdxK0s;
            mother = GetVParticleMother(mcpart);
            if(mother){
                if((abs(mother->PdgCode()) == bPhi))
                    return kFALSE;
            }
            return kTRUE;
            break;

            case bLambda:
            idx = bIdxLambda;
            mother = GetVParticleMother(mcpart);
            if(mother){
                if((abs(mother->PdgCode()) ==  3312) || (abs(mother->PdgCode()) ==  3322) || (abs(mother->PdgCode()) ==  3334))
                    return kFALSE;
            }
            return kTRUE;
            break;
            default:
            return kFALSE;
            break;
        }
        return kFALSE;
    }
/*! \brief IsPromptBMeson
 *
 *
 */
    Bool_t AliAnalysisTaskHFJetIPQA::IsPromptBMeson(AliVParticle * part )
    {
        if(!part) return kFALSE;
        Int_t pdg = TMath::Abs(part->PdgCode());
        if ((pdg >= 500 && pdg < 600 )||(pdg >= 5000 && pdg < 6000 ))
        {
            AliVParticle* pm = GetVParticleMother(part);
            Int_t mpdg = TMath::Abs(pm->PdgCode());
            if (!(mpdg >5000 && mpdg <6000) && !(mpdg >500 && mpdg <600))
                return kTRUE;
        }
        return kFALSE;
    }
/*! \brief IsPromptDMeson
 *
 *
 */
    Bool_t AliAnalysisTaskHFJetIPQA::IsPromptDMeson(AliVParticle * part )
    {
        if(!part) return kFALSE;
        Int_t pdg = TMath::Abs(part->PdgCode());
        if ((pdg >= 400 && pdg < 500 )||(pdg >= 4000 && pdg < 5000 ))
        {
            AliVParticle* pm = GetVParticleMother(part);
            if(!pm) return kTRUE;
            Int_t mpdg = TMath::Abs(pm->PdgCode());
            if (!(mpdg >4000 && mpdg <6000) && !(mpdg >400 && mpdg <600))
                return kTRUE;
        }

        return kFALSE;
    }
/*! \brief ParticleIsPossibleSource
 *
 *
 */
    Bool_t AliAnalysisTaskHFJetIPQA::ParticleIsPossibleSource(Int_t pdg){
        Int_t pos[22] = {bPi0,bEta,bEtaPrime,bPhi,bRho,bOmega,bK0s,bLambda,bOmegaBaryon,bXiBaryon,bD0,bPi,bKaon,bProton,bDPlus,bDStarPlus,bDSPlus,bLambdaB,bLambdaC,bBPlus,bB0,bBStarPlus};
        for (Int_t i =0 ;i<22 ;++i){
            if (abs(pdg)==pos[i] ) return kTRUE;
        }
        return kFALSE;
    }
/*! \brief SetMatchingLevel
 *
 * jet matching helper:
 * - define closest and second closest jet 
 */
    void AliAnalysisTaskHFJetIPQA::SetMatchingLevel(AliEmcalJet *jet1, AliEmcalJet *jet2, Int_t matching)
    {
        Double_t d1 = - 1;
        Double_t d2 = -1;

        switch (matching) {
            case 1:
            GetGeometricalMatchingLevel(jet1,jet2,d1);
            d2 = d1;
            break;
            default:
            break;
        }
        if (d1 >= 0) {

            if (d1 < jet1->ClosestJetDistance()) {
                jet1->SetSecondClosestJet(jet1->ClosestJet(), jet1->ClosestJetDistance());
                jet1->SetClosestJet(jet2, d1);
            }
            else if (d1 < jet1->SecondClosestJetDistance()) {
                jet1->SetSecondClosestJet(jet2, d1);
            }
        }
        if (d2 >= 0) {
            if (d2 < jet2->ClosestJetDistance()) {
                jet2->SetSecondClosestJet(jet2->ClosestJet(), jet2->ClosestJetDistance());
                jet2->SetClosestJet(jet1, d2);
            }
            else if (d2 < jet2->SecondClosestJetDistance()) {
                jet2->SetSecondClosestJet(jet1, d2);
            }
        }
    }
/*! \brief GetGeometricalMatchingLevel
 *
 * jet matching helper
 */
    void AliAnalysisTaskHFJetIPQA::GetGeometricalMatchingLevel(AliEmcalJet *jet1, AliEmcalJet *jet2, Double_t &d) const
    {
        Double_t deta = jet2->Eta() - jet1->Eta();
        Double_t dphi = jet2->Phi() - jet1->Phi();
        dphi = TVector2::Phi_mpi_pi(dphi);
        d = sqrt(deta * deta + dphi * dphi);
    }
/*! \brief GetMonteCarloCorrectionFactor
 *
 * Composition  correction base function caller
 */
    Double_t AliAnalysisTaskHFJetIPQA::GetMonteCarloCorrectionFactor(AliVTrack* track,Int_t &pCorr_indx, double &ppt){
        printf("Doing MC Correction.\n");
        double val=  GetWeightFactor(track,pCorr_indx,ppt);
        if(val > 0 ) return val;
        return 1.;
    }
/*! \brief mysort
 *
 * custom strcut sorter function
 */
    Bool_t AliAnalysisTaskHFJetIPQA::mysort(const SJetIpPati& i, const SJetIpPati& j)
    {
        if(i.first <= j.first)
            return kFALSE;
        else
            return kTRUE;
    }
/*! \brief GetPtCorrected
 *
 *
 */
    Double_t AliAnalysisTaskHFJetIPQA::GetPtCorrected(const AliEmcalJet *jet)
    {
        AliJetContainer * jetconrec = nullptr;
        jetconrec = static_cast<AliJetContainer*>(fJetCollArray.At(0));
        if(jet && jetconrec&&fDoUnderlyingEventSub){
            printf("Correct for Underlying Event.\n");
            return jet->Pt() - jetconrec->GetRhoVal() * jet->Area();
        }
        return -1.;
    }
/*! \brief GetPtCorrectedMC
 *
 *
 */
    Double_t AliAnalysisTaskHFJetIPQA::GetPtCorrectedMC(const AliEmcalJet *jet)
    {
        AliJetContainer * jetconrec = nullptr;
        jetconrec = static_cast<AliJetContainer*>(fJetCollArray.At(1));
        if(jet && jetconrec&&fDoUnderlyingEventSub){
            printf("Correct for Underlying Event.\n");
            return jet->Pt() - jetconrec->GetRhoVal() * jet->Area();
        }
        return -1.;
    }


/*! \brief IsJetTaggedTC
 * unused
 *
 */
    Bool_t AliAnalysisTaskHFJetIPQA::IsJetTaggedTC(Int_t n, Double_t thres)
    {
        return kTRUE;
    }


/*! \brief IsParton
 *
 *
 */
    Bool_t AliAnalysisTaskHFJetIPQA::IsParton(int pdg){
      return ((pdg==1)||(pdg==2)||(pdg==3)||(pdg==4)||(pdg==5)||(pdg==21));
    }

/*! \brief IsJetTaggedJetProb
 *
 * unused
 */
    Bool_t AliAnalysisTaskHFJetIPQA::IsJetTaggedJetProb(Double_t thresProb)
    {
        return kTRUE;
    }
    /*! \brief IsMCJetPartonFast
     *
     * Fast jet parton MC matcher
     */
        Int_t  AliAnalysisTaskHFJetIPQA::IsMCJetPartonFast(const AliEmcalJet *jet, Double_t radius,Bool_t &is_udg)
        {
            fJetCont.clear();
            fPUdsgJet.clear();
            fPSJet.clear();
            fPCJet.clear();
            fPBJet.clear();
            daughtermother.clear();

            double p_udsg_max=-999;
            double p_s_max=-999;
            AliVParticle *vp=0x0;
            Int_t pdg=0;
            Double_t p=0;
            int kJetOrigin=-999;

            //printf("XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX\n");
            //printf("Starting loop\n");
            if(!jet) return 0;
            if(!(jet->GetNumberOfTracks()>fNoJetConstituents)){
              //printf("Throwing away jets with only too few constituent!\n");
              return 0;
            }

            if(fDoFlavourMatching){
              for(UInt_t i = 0; i < jet->GetNumberOfTracks(); i++) {//start trackloop jet
                vp = static_cast<AliVParticle*>(jet->Track(i));
                if (!vp){
                  AliError("AliVParticle associated to constituent not found\n");
                  continue;
                }

                AliAODMCParticle * part = static_cast<AliAODMCParticle*>(vp);

                if(!part){
                    AliError("Finding no Part!\n");
                    return 0;
                }       // if(!part->IsPrimary()) continue;
                pdg = (abs(part->PdgCode()));
                fJetCont.push_back(part->Label());
                //printf("Daugther pdg=%i, Label=%i, Mother =%i, p=%f, MCStatusCode=%i\n",pdg, part->GetLabel(), part->GetMother(), p, part->MCStatusCode());
              }//end trackloop jet
            }

            for(Int_t iPrim = 0 ; iPrim<fMCArray->GetEntriesFast();iPrim++){//start trackloop MC

                        AliAODMCParticle * part = static_cast<AliAODMCParticle*>(fMCArray->At(iPrim));
                        if(!part) return 0;
                        if(!part->IsPrimary()) continue;
                        Double_t eta = part->Eta();
                        Double_t phi= part->Phi();
                        Double_t etajet = jet->Eta();
                        Double_t phijet = jet->Phi();
                         p=part->P();

                        Int_t pdg = (abs(part->PdgCode()));
                        Double_t deta = etajet - eta;
                        Double_t dphi = phijet-phi;
                        dphi = TVector2::Phi_mpi_pi(dphi);
                        Double_t  d = sqrt(deta * deta + dphi * dphi);
                      //   printf("LINE %i, deta%f, dphi%f, d=%f, fDoFlavourMatching=%i\n",__LINE__, deta,dphi,d,fDoFlavourMatching);
                        if(!fDoFlavourMatching) {
                          //if(!((part->GetStatus()==11) ||(part->GetStatus()==12))) continue;
                          if(!IsParton(pdg)) continue;
                          if(d > radius) continue;
                          kJetOrigin=pdg;
                        }
                        else{

                          if(IsParton(pdg)){
                            if(d > fDaughtersRadius) continue;
                          }

                          //printf("i=%i, Mother Label %i, Mother PdG %i,Daughter:%i, Last Daughter: %i, MCStatusCode=%i, d=%f, p=%f\n", iPrim, part->Label(), part->PdgCode(), part->GetDaughterLabel(0),part->GetDaughterLabel(1),part->MCStatusCode(), d,p);

                          int kFirstDaugh=part->GetDaughterLabel(0);
                          int NDaugh=part->GetNDaughters();
                          for(int idaugh=0;idaugh<NDaugh;idaugh++){
                            int kDaughLabel=kFirstDaugh+idaugh;
                            //printf("Dauglabel=%i, kFirstDaugh=%i, kLastDaugh=%i\n",kDaughLabel, kFirstDaugh,kLastDaugh);

                            bool IsDaughter=std::find(fJetCont.begin(), fJetCont.end(),kDaughLabel) != fJetCont.end();
                            if(IsDaughter){
                              if(IsParton(pdg)){
                                //printf("Directly matched %i with daughter =%i\n",part->GetLabel(), kDaughLabel);
                                kJetOrigin=part->PdgCode();
                              }
                              else{
                                  bool Is2ndDaughter=daughtermother.find(part->Label()) != daughtermother.end();
                                  if(Is2ndDaughter){
                                      kJetOrigin=daughtermother.find(part->Label())->second;
                                      //printf("Matched with %i with 2nd daughter =%i\n",part->GetLabel(), kDaughLabel);
                                  }
                              }
                            }//end is jet daughter
                            else{
                              if(IsParton(pdg)){
                                //printf("Writing Quarks in map: partlabel=%i, daughlabel=%i, status=%i\n", part->GetLabel(),kDaughLabel, part->MCStatusCode());
                                daughtermother.emplace(kDaughLabel, part->PdgCode());
                              }
                              else{
                                bool Is2ndDaughter=daughtermother.find(part->Label()) != daughtermother.end();
                                if(Is2ndDaughter){
                                  //printf("Writing Daughters in map: partlabel=%i, daughlabel=%i, status=%i\n", part->GetLabel(),kDaughLabel, part->MCStatusCode());
                                  int kOriginalQuark=daughtermother.find(part->Label())->second;
                                  daughtermother.emplace(kDaughLabel, kOriginalQuark);
                                }
                              //printf("Asking for daughlabel%i\n",part->Label());
                              }
                            }//end related to jet?
                          }//end daughterloop
                        }//end else DoMatchFlavours

                        //printf("i=%i, Mother Label %i, Mother PdG %i,Daughter:%i, Last Daughter: %i, MCStatusCode=%i, d=%f, p=%f\n", iPrim, part->Label(), part->PdgCode(), part->GetDaughterLabel(0),part->GetLastDaughter(),part->MCStatusCode(), d,p);

                        if(abs(kJetOrigin) == 5) {
                            fPBJet.push_back(p);
                        }
                        else if(abs(kJetOrigin)== 4) {
                            fPCJet.push_back(p);
                        }
                        else if(abs(kJetOrigin) == 3 ) {
                            fPSJet.push_back(p);
                            //printf("Strange pushed with p=%f",p);
                        }
                        else if(abs(kJetOrigin)== 1 ||abs(kJetOrigin)== 2 ||  abs(kJetOrigin) == 21) {
                            fPUdsgJet.push_back(p);
                            //printf("Light pushed with p=%f",p);
                        }

        }//end trackloop MC

        /*printf("Inside JetCont:\n");
          for(int i=0;i<fJetCont.size();i++){
          printf("%f\n", fJetCont[i]);
        }
        printf("Inside map:\n");
          for (auto& x: daughtermother) {
          std::cout << x.first << ": " << x.second << '\n';
        }*/
        if(fPCJet.size() ==0&& fPBJet.size()==0&& fPSJet.size()==0&&fPUdsgJet.size()==0) return 0; //udsg
        //check for b jet
        for (Int_t icj = 0 ; icj <(Int_t)fPBJet.size();++icj ){
            //printf("Bottom Flavour Jet!\n");
            return 3;
        }
        //check for c jet
        for (Int_t icj = 0 ; icj <(Int_t)fPCJet.size();++icj ){
            //printf("Charm Flavour Jet!\n");
            return 2;
        }
        //check for s and light jet
        if(fPUdsgJet.size()!=0){
          std::sort(fPUdsgJet.begin(), fPUdsgJet.end());
          p_udsg_max=fPUdsgJet[fPUdsgJet.size()-1];
        }
        if(fPSJet.size()!=0){
          std::sort(fPSJet.begin(), fPSJet.end());
          p_s_max=fPSJet[fPSJet.size()-1];
        }

        if(p_s_max>p_udsg_max){
          //printf("S prefered with psmax=%f, pudsgmax=%f\n", p_s_max,p_udsg_max);
          return 4;
        }
        else{
            if(fPUdsgJet.size()!=0){
                //printf("Light prefered with psmax=%f, pudsgmax=%f\n", p_s_max,p_udsg_max);
                is_udg =kTRUE;
                return 1;
            }
        }
        return 0;
    }


/*! \brief RecursiveParents
 *
 * function which is declustering jets via Camebridge Aachen algorithm and from subjets filling the Lund plane
  */
//_________________________________________________________________________
void AliAnalysisTaskHFJetIPQA::RecursiveParents(AliEmcalJet *fJet,AliJetContainer *fJetCont){

      std::vector<fastjet::PseudoJet>  fInputVectors;
      fInputVectors.clear();
      fastjet::PseudoJet  PseudoTracks;

      AliParticleContainer *fTrackCont = fJetCont->GetParticleContainer();

        if (fTrackCont) for (Int_t i=0; i<fJet->GetNumberOfTracks(); i++) {
          AliVParticle *fTrk = fJet->TrackAt(i, fTrackCont->GetArray());
          if (!fTrk) continue;
          //if(fDoTwoTrack==kTRUE && CheckClosePartner(i,fJet,fTrk,fTrackCont)) continue;
          PseudoTracks.reset(fTrk->Px(), fTrk->Py(), fTrk->Pz(),fTrk->E());
          PseudoTracks.set_user_index(fJet->TrackAt(i)+100);
          fInputVectors.push_back(PseudoTracks);

        }
        fastjet::JetAlgorithm jetalgo(fastjet::cambridge_algorithm);



      fastjet::JetDefinition fJetDef(jetalgo, 1., static_cast<fastjet::RecombinationScheme>(0), fastjet::BestFJ30 );

      try {
        fastjet::ClusterSequence fClustSeqSA(fInputVectors, fJetDef);
        std::vector<fastjet::PseudoJet>   fOutputJets;
        fOutputJets.clear();
        fOutputJets=fClustSeqSA.inclusive_jets(0);

       fastjet::PseudoJet jj;
       fastjet::PseudoJet j1;
       fastjet::PseudoJet j2;
       jj=fOutputJets[0];
       double ktaverage=0;
       double thetaverage=0;
       double nall=0;
       double flagSubjet=0;
        while(jj.has_parents(j1,j2)){
          nall=nall+1;
        if(j1.perp() < j2.perp()) swap(j1,j2);
        flagSubjet=0;
        double delta_R=j1.delta_R(j2);
        double z=j2.perp()/(j1.perp()+j2.perp());
        double y =log(1.0/delta_R);
        double lnpt_rel=log(j2.perp()*delta_R);
        double yh=j1.e()+j2.e();
         vector < fastjet::PseudoJet > constitj1 = sorted_by_pt(j1.constituents());
         if(constitj1[0].perp()>fAnalysisCuts[bAnalysisCut_MinTrackPt]) flagSubjet=1;
        if(z>fHardCutOff){
          ktaverage=ktaverage+lnpt_rel;
          thetaverage=thetaverage+delta_R;
        Double_t LundEntries[6] = {y,lnpt_rel,fOutputJets[0].perp(),nall,yh,flagSubjet};
        fHLundIterative->Fill(LundEntries);}
        jj=j1;}
      } catch (fastjet::Error) {
        AliError(" [w] FJ Exception caught.");
        //return -1;
      }
      return;
}
/*! \brief FillHist
 *
 * 1d
 */
void AliAnalysisTaskHFJetIPQA::FillHist(const char *name, Double_t x, Double_t w){
    TH1D * h1 =GetHist1D(name);
    if(h1)  h1->Fill(x,w);
}
/*! \brief FillHist
 *
 * 2d
 */
void AliAnalysisTaskHFJetIPQA::FillHist(const char *name, Double_t x, Double_t y, Double_t w){
    TH2D * h2 =GetHist2D(name);
    if(h2) h2->Fill(x,y,w);
}
/*! \brief IncHist
 *
 * increase 1d hist bin
 */
void AliAnalysisTaskHFJetIPQA::IncHist(const char *name, Int_t bin){
    TH1D * h1 =GetHist1D(name);
    h1->SetBinContent(bin,h1->GetBinContent(bin)+1);
}
/*! \brief AddHistogramm
 *
 *add a historgram
 */
TH1 *AliAnalysisTaskHFJetIPQA::AddHistogramm(const char *name, const char *title, Int_t x, Double_t xlow, Double_t xhigh, Int_t y, Double_t ylow, Double_t yhigh){
    TObject * res = nullptr;
    res = fOutput->FindObject(name);
    if((res)) return nullptr;

    TH1 * phist=nullptr;
    if(y==0){ //TH1D*
        phist = new TH1D (name,title,x,xlow,xhigh);
    }
    else  {
        phist = new TH2D(name,title,x,xlow,xhigh,y,ylow,yhigh);
    }
    phist->Sumw2();

    TString sName(name);
    if(sName.EqualTo("fh1dCutsPrinted")){
        fOutput->AddFirst(phist);
    }
    else{
        fOutput->Add(phist);
    }
    return (TH1*)phist;
}

void AliAnalysisTaskHFJetIPQA::setFFillCorrelations(const Bool_t &value)
{
    fFillCorrelations = value;
}



void AliAnalysisTaskHFJetIPQA::setFMCglobalDCASmear(const Double_t value)
{
    fMCglobalDCASmear = value;
}

Double_t AliAnalysisTaskHFJetIPQA::getFVertexRecalcMinPt() const
{
    return fVertexRecalcMinPt;
}

void AliAnalysisTaskHFJetIPQA::setFVertexRecalcMinPt(const Double_t &value)
{
    fVertexRecalcMinPt = value;
}

Double_t AliAnalysisTaskHFJetIPQA::getFMCglobalDCAxyShift() const
{
    return fMCglobalDCAxyShift;
}

void AliAnalysisTaskHFJetIPQA::setFMCglobalDCAxyShift(const Double_t &value)
{
    fMCglobalDCAxyShift = value;
}





/*! \brief SubtractMean
 *
 *
 */


//Currently not in use
/*void AliAnalysisTaskHFJetIPQA::SubtractMean(Double_t val[], AliVTrack *track){
    Double_t  deltamean=fGraphMean->Eval(track->Pt() < 3. ? track->Pt() : 3 );//(fCurrentMeanFactors[0]-fCurrentMeanFactors[1]*TMath::Exp(-1*fCurrentMeanFactors[2]*(track->Pt()-fCurrentMeanFactors[3]))) *1e-4;

    val[0] -=deltamean*1e-4;;
}
//Helpers*/


Bool_t AliAnalysisTaskHFJetIPQA::GetImpactParameter(const AliAODTrack *track, const AliAODEvent *event, Double_t *dca, Double_t *cov, Double_t *XYZatDCA)
{
    if(!track || !event) return kFALSE;
    if(dca==0 || cov ==0 ||XYZatDCA ==0 ) return kFALSE;
    AliExternalTrackParam etp;
    etp.CopyFromVTrack((AliVTrack*)track);

    const Double_t kBeampiperadius=3;  //maximal dca used for track propagation
    fEventVertex = RemoveDaughtersFromPrimaryVtx(track);
    if(!fEventVertex)return kFALSE;
    //printf("After remove:\n");
    //fEventVertex->Print();
    const AliVVertex *vtxESDSkip =fEventVertex;//RemoveDaughtersFromPrimaryVtx(track);
    if(!vtxESDSkip) return kFALSE;
    if(!etp.PropagateTo(1,event->GetMagneticField())) return kFALSE;
    Bool_t success = kFALSE;
    Double_t x_at_dca[3];
    Double_t p_at_dca[3];

    //Classic dca calculation
    if(etp.PropagateToDCA(vtxESDSkip, event->GetMagneticField(), kBeampiperadius, dca, cov)){
        success = kTRUE;
        etp.GetXYZ(XYZatDCA);
        etp.GetXYZ(x_at_dca);
        etp.GetPxPyPz(p_at_dca);
            //         if(fIsPythia)   dca[0] *= fMCglobalDCASmear;
            //         if(fIsPythia)   dca[0] += fMCglobalDCAxyShift; // generic mean offset in LHC10e default is 0.007 == 7 µm

    } else return kFALSE;
    return success;
}

/*AliExternalTrackParam AliAnalysisTaskHFJetIPQA::GetExternalParamFromJet(const AliEmcalJet *jet, const AliAODEvent *event)
{
    double vtx[3]= {0.};
    double cov [21] = {0.};
    double pxpypz[3] = {0.};
    jet->PxPyPz(pxpypz);
    (event->GetPrimaryVertex())->GetXYZ(vtx);
    AliExternalTrackParam etp     (vtx, pxpypz, cov, (Short_t)0);
    return etp;
}*/



Bool_t AliAnalysisTaskHFJetIPQA::getJetVtxMass(AliEmcalJet *jet,double &value ){

    return true;



    const AliVVertex * vertex = InputEvent()->GetPrimaryVertex();

    Printf("Primary vertex %e %e %e",vertex->GetX(),vertex->GetY(),vertex->GetY() );
    /*
    if(!amvf)  amvf = new AliHFAdaptiveMVF(InputEvent()->GetMagneticField());
    amvf->_fitter->_seeder->_vertex_penalty((AliAODVertex*)InputEvent()->GetPrimaryVertex());
    amvf->_fitter->_r_fitter_jet(jet,(AliAODEvent*)InputEvent());
    return kTRUE;*/
}


Bool_t AliAnalysisTaskHFJetIPQA::GetImpactParameterWrtToJet(const AliAODTrack *track, const AliAODEvent *event, const AliEmcalJet *jet, Double_t *dca, Double_t *cov, Double_t *XYZatDCA, Double_t &jetsign, int jetflavour)
{
    if(!track || !event || !jet)return kFALSE;
    if(dca==0 || cov ==0 ||XYZatDCA ==0 ) return kFALSE;

    if(!GetImpactParameter(track,event,dca,cov,XYZatDCA)) return kFALSE;
    //vertex properties
    Double_t VxVyVz[3]= {0.,0.,0.};
    const  AliVVertex *vtxESDSkip =fEventVertex;    //GetImpactParameter does set fEventVertex to the recalculated one
    if(!fEventVertex) return kFALSE;
    vtxESDSkip->GetXYZ(VxVyVz);
    //printf("Vertex in wrt jet:\n");
    //vtxESDSkip->Print();

    //jet properties
    double jetp[3];
    jet->PxPyPz(jetp);
    TVector3 jetP3(jetp);
    double covjet [21] = {0.};
    double pxpypz[3] = {0.};
    jet->PxPyPz(pxpypz);
    AliExternalTrackParam etp_jet(VxVyVz, pxpypz, covjet, (Short_t)0);

    //Calculation of sign
    TVector3 JetDir =jetP3.Unit();
    TVector3 D0(XYZatDCA);
    TVector3 vertex(VxVyVz);
    TVector3 DD0(D0.x()-vertex.x(),D0.y()-vertex.y(),0.);   //track impact parameter
    double ps =DD0.Dot(JetDir);
    double value = DD0.Mag()*(ps/fabs(ps));                 //absolut track impact parameter
    jetsign  = TMath::Sign(1.,value);                       //sign
    TVector3 dd0 =DD0.Unit();

    //track properties
    AliExternalTrackParam etp_track;    etp_track.CopyFromVTrack(track);
    Double_t xa,xb,xyz_jet_global[3],xyz_track_global[3];

    etp_jet.GetDCA(&etp_track, event->GetMagneticField(), xa, xb);
    etp_jet.GetXYZAt(xa, event->GetMagneticField(),xyz_jet_global);
    etp_track.GetXYZAt(xb, event->GetMagneticField(),xyz_track_global);
    etp_track.PropagateTo(xb,event->GetMagneticField());

    if(fEventVertex) {
        delete fEventVertex;
        fEventVertex =nullptr;

    }

    double val = ((VxVyVz[0] - xyz_track_global[0]) * (VxVyVz[0] - xyz_track_global[0]) +
        (VxVyVz[1] - xyz_track_global[1]) * (VxVyVz[1] - xyz_track_global[1])+
        (VxVyVz[2] - xyz_track_global[2]) * (VxVyVz[2] - xyz_track_global[2]));


    double  bdecaylength = val >0 ? sqrt(val) : 1000;
    //printf("decaylength:\n");
    //for(int i=0;i<3;i++){
    //    printf("VxVyVZ=%f, xyztrackglobal=%f\n",VxVyVz[i], xyz_track_global[i]);
    //}

    double dcajetrack = sqrt((xyz_jet_global[0] - xyz_track_global[0]) * (xyz_jet_global[0] - xyz_track_global[0]) +
        (xyz_jet_global[1] - xyz_track_global[1]) * (xyz_jet_global[1] - xyz_track_global[1])+
        (xyz_jet_global[2] - xyz_track_global[2]) * (xyz_jet_global[2]- xyz_track_global[2]));

    //printf("decaylength:\n");
    //for(int i=0;i<3;i++){
    //    printf("xyzjetglobal=%f, xyztrackglobal=%f\n",xyz_jet_global[i], xyz_track_global[i]);
    //}

    if(!(IsDCAAccepted(bdecaylength, dcajetrack, dca, jetflavour))) return kFALSE;
    //printf("decaylength=%f, pi=%f\n", bdecaylength, dcajetrack);

    return kTRUE;
}


void AliAnalysisTaskHFJetIPQA::SetThresholds(int nthresh, TObjArray** &threshs){
  for(int iProbSet=0;iProbSet<nthresh;iProbSet++){
    TObjArray* oa=(TObjArray*)threshs[iProbSet];

    //printf("Pointer oa=%p\n",oa);

    h1DThresholdsFirst.push_back((TH1D*)oa->At(0));
    h1DThresholdsSecond.push_back((TH1D*)oa->At(1));
    h1DThresholdsThird.push_back((TH1D*)oa->At(2));

    TString sFrac=h1DThresholdsFirst[iProbSet]->GetTitle();
    fFracs.push_back(sFrac.Atof());
  }
  //checking fFracs
  //for(int iFrac=0;iFrac<fFracs.size();iFrac++){
  //    printf("iFrac=%i, %f\n",iFrac,fFracs[iFrac]);
  //}
  /*  int nPoints=h1DThresholdsFirst[0]->GetNbinsX();

    printf("LargestIP: 2nd Histogram bins:\n");
    for(int iPoint=0;iPoint<nPoints;iPoint++){
      printf("iPoint=%i, xval=%f, yval=%f\n",iPoint, h1DThresholdsFirst[1]->GetBinCenter(iPoint),h1DThresholdsFirst[1]->GetBinContent(iPoint));
    }
    printf("2ndLargestIP: 3rd Histogram bins:\n");
    for(int iPoint=0;iPoint<nPoints;iPoint++){
      printf("iPoint=%i, xval=%f, yval=%f\n",iPoint, h1DThresholdsSecond[2]->GetBinCenter(iPoint),h1DThresholdsSecond[2]->GetBinContent(iPoint));
    }
    printf("3rdLargestIP: 1st Histogram bins:\n");
    for(int iPoint=0;iPoint<nPoints;iPoint++){
      printf("iPoint=%i, xval=%f, yval=%f\n",iPoint, h1DThresholdsThird[0]->GetBinCenter(iPoint),h1DThresholdsThird[0]->GetBinContent(iPoint));
    }*/
}

void AliAnalysisTaskHFJetIPQA::ReadProbvsIPLookup(TObjArray*& oLookup){

  for(int iN=0;iN<3;iN++){
    h2DProbLookup.push_back((TH2D*)oLookup->At(iN));
  }
}

void AliAnalysisTaskHFJetIPQA::DoJetTaggingThreshold(double jetpt, bool* hasIPs, double* ipval, bool **kTagDec){
  //threshold values for tracks with largest, second and third largest IP
  int iJetPtBin=h1DThresholdsFirst[0]->FindBin(jetpt);
  double IPthresN1[fNThresholds];  //IP threshold values for individual separation power
  double IPthresN2[fNThresholds];
  double IPthresN3[fNThresholds];

  for(int iN=0;iN<fNThresholds;iN++){
    IPthresN1[iN]=h1DThresholdsFirst[iN]->GetBinContent(iJetPtBin);
    IPthresN2[iN]=h1DThresholdsSecond[iN]->GetBinContent(iJetPtBin);
    IPthresN3[iN]=h1DThresholdsThird[iN]->GetBinContent(iJetPtBin);
  }



  for(int iThresh=0;iThresh<fNThresholds;iThresh++){
    if(!hasIPs[0]) continue;
    //printf("DoJetTaggingThreshold:\n");
    //printf("      iJetPtBin=%i, IPthresN1=%f, IPthresN2=%f, IPthresN3=%f\n", iJetPtBin, IPthresN1[iThresh],IPthresN2[iThresh], IPthresN3[iThresh]);


    if(hasIPs[2]){
      //tripple tag
      //printf("ipval[0]=%f, ipval[1]=%f, ipval[2]=%f\n", ipval[0],ipval[1],ipval[2]);
      if(ipval[0]>IPthresN1[iThresh]&&ipval[1]>IPthresN2[iThresh]&&ipval[2]>IPthresN3[iThresh]){
          //printf("Triple %f!\n",fFracs[iThresh]);
          kTagDec[iThresh][Full]=kTRUE; kTagDec[iThresh][Triple]=kTRUE;
      }

      //single tag
      if(kTagLevel<2){
        //printf("Single catch\n");
        if(ipval[2]>IPthresN3[iThresh]) {
        //printf("Single3rd %f!\n",fFracs[iThresh]);
          kTagDec[iThresh][Full]=kTRUE; kTagDec[iThresh][Single3rd]=kTRUE;
        }
      }
    }

    if(hasIPs[1]){
      //printf("ipval[0]=%f, ipval[1]=%f\n", ipval[0],ipval[1]);
      //double tag
      if(kTagLevel<3){
        //printf("Double catch\n");
        if(ipval[0]>IPthresN1[iThresh]&&ipval[1]>IPthresN2[iThresh]) {
          //printf("Double %f!\n",fFracs[iThresh]);
          kTagDec[iThresh][Full]=kTRUE; kTagDec[iThresh][Double]=kTRUE;
        }
      }
      //single tag
      if(kTagLevel<2){
        //printf("Single catch\n");
        if(ipval[1]>IPthresN2[iThresh]) {
          //printf("Single2nd %f!\n",fFracs[iThresh]);
          kTagDec[iThresh][Full]=kTRUE; kTagDec[iThresh][Single2nd]=kTRUE;
        }
      }
    }

    //single tag
    if(hasIPs[0]){
      //printf("ipval[0]=%f", ipval[0]);
      if(kTagLevel<2){
        //printf("Single catch\n");
        if(ipval[0]>IPthresN1[iThresh]) {
          //printf("Single1st %f!\n",fFracs[iThresh]);
          kTagDec[iThresh][Full]=kTRUE; kTagDec[iThresh][Single1st]=kTRUE;
        }
      }
    }
   /* printf("Testing kTagLevel::\n ");
    for(int iThresh=0;iThresh<fNThresholds;iThresh++){
      for(int iType=0;iType<6;iType++){
        printf("iThresh=%f, %i, kTagDec=%i\n",fFracs[iThresh],iType,kTagDec[iThresh][iType]);
      }
    }*/
  }
}

void AliAnalysisTaskHFJetIPQA::FillTCEfficiencyHists(bool** kTagDec, int jetflavour, double jetpt, bool hasIPs){
  //printf("Receiving BTagged decision: %i\n", kTagDec[Full]);
  for(int iThresh=0;iThresh<fNThresholds;iThresh++){
    //printf("kTagDec=%i, jetflavour=%i, hasIPs=%i\n",kTagDec[iThresh][Full],jetflavour, hasIPs);
    if(kTagDec[iThresh][Full])FillHist(Form("h1DTagged_%0.2f",fFracs[iThresh]),jetpt,1);

    if(kTagDec[iThresh][Full]&&(jetflavour==3)&&hasIPs){
      //printf("################################ Before: FoundJet with tagindex=%i!\n",kTagDec[iThresh][Full]);

      FillHist(Form("h1DTrueBTagged_Full_%0.2f",fFracs[iThresh]), jetpt, 1);
      if(kTagDec[iThresh][Single1st]) FillHist(Form("h1DTrueBTagged_Single1st_%0.2f",fFracs[iThresh]), jetpt, 1);
      if(kTagDec[iThresh][Single2nd]) FillHist(Form("h1DTrueBTagged_Single2nd_%0.2f",fFracs[iThresh]), jetpt, 1);
      if(kTagDec[iThresh][Single3rd])FillHist(Form("h1DTrueBTagged_Single3rd_%0.2f",fFracs[iThresh]), jetpt, 1);
      if(kTagDec[iThresh][Double])FillHist(Form("h1DTrueBTagged_Double_%0.2f",fFracs[iThresh]), jetpt, 1);
      if(kTagDec[iThresh][Triple])FillHist(Form("h1DTrueBTagged_Triple_%0.2f",fFracs[iThresh]), jetpt, 1);

      //printf("################################ FoundJet with tagindex=%i!\n",kTagDec[iThresh][Full]);
    }
    if(kTagDec[iThresh][Full]&&(jetflavour==2)&&hasIPs){
      FillHist(Form("h1DFalseCTagged_Full_%0.2f",fFracs[iThresh]), jetpt, 1);
      if(kTagDec[iThresh][Single1st]) FillHist(Form("h1DFalseCTagged_Single1st_%0.2f",fFracs[iThresh]), jetpt, 1);
      if(kTagDec[iThresh][Single2nd]) FillHist(Form("h1DFalseCTagged_Single2nd_%0.2f",fFracs[iThresh]), jetpt, 1);
      if(kTagDec[iThresh][Single3rd]) FillHist(Form("h1DFalseCTagged_Single3rd_%0.2f",fFracs[iThresh]), jetpt, 1);
      if(kTagDec[iThresh][Double])FillHist(Form("h1DFalseCTagged_Double_%0.2f",fFracs[iThresh]), jetpt, 1);
      if(kTagDec[iThresh][Triple])FillHist(Form("h1DFalseCTagged_Triple_%0.2f",fFracs[iThresh]), jetpt, 1);
      //printf("################################ CMistagged: flavour is=%i with tagindex=%i!\n",jetflavour,kTagDec[iThresh][Full]);
    }
    if(kTagDec[iThresh][Full]&&(jetflavour==1||jetflavour==4)&&hasIPs){
      FillHist(Form("h1DFalseUDSGTagged_Full_%0.2f",fFracs[iThresh]), jetpt, 1);
      if(kTagDec[iThresh][Single1st]) FillHist(Form("h1DFalseUDSGTagged_Single1st_%0.2f",fFracs[iThresh]), jetpt, 1);
      if(kTagDec[iThresh][Single2nd]) FillHist(Form("h1DFalseUDSGTagged_Single2nd_%0.2f",fFracs[iThresh]), jetpt, 1);
      if(kTagDec[iThresh][Single3rd]) FillHist(Form("h1DFalseUDSGTagged_Single3rd_%0.2f",fFracs[iThresh]), jetpt, 1);
      if(kTagDec[iThresh][Double])FillHist(Form("h1DFalseUDSGTagged_Double_%0.2f",fFracs[iThresh]), jetpt, 1);
      if(kTagDec[iThresh][Triple])FillHist(Form("h1DFalseUDSGTagged_Triple_%0.2f",fFracs[iThresh]), jetpt, 1);
      //printf("################################ LightMistagged: flavour is=%i with tagindex=%i!\n",jetflavour,kTagDec[iThresh][Full]);
    }
    if(!kTagDec[iThresh][Full]&&jetflavour==3&&hasIPs){
      //printf("################################ Missed one: flavour is=%i\n", jetflavour);
    }
  }
}

double AliAnalysisTaskHFJetIPQA::IntegrateIP(int iJetPtBin, int iIPBin, int iN){
  int iZeroIPBin=h2DProbLookup[iN]->GetXaxis()->FindBin(0.);
  int iStartIPBin=h2DProbLookup[iN]->GetXaxis()->FindBin(-25);

  double prob=h2DProbLookup[iN]->Integral(iStartIPBin,iIPBin,iJetPtBin,iJetPtBin);
  prob=prob/(h2DProbLookup[iN]->Integral(iStartIPBin,iZeroIPBin,iJetPtBin,iJetPtBin));

  //printf("Integrate: 0x=%f, lowx=%f, upx=%f, lowy=%f, upy=%f, prob=%f\n", h2DProbLookup[iN]->GetXaxis()->GetBinLowEdge(iZeroIPBin), h2DProbLookup[iN]->GetXaxis()->GetBinLowEdge(iStartIPBin),h2DProbLookup[iN]->GetXaxis()->GetBinLowEdge(iIPBin),h2DProbLookup[iN]->GetYaxis()->GetBinLowEdge(iJetPtBin),h2DProbLookup[iN]->GetYaxis()->GetBinLowEdge(iJetPtBin+1),prob);

  return prob;
}

double AliAnalysisTaskHFJetIPQA::GetTrackProbability(double jetpt, bool* hasIPs, double* ipval){
  //printf("Printing h2DProbLook\n");
  double prob=1;
  double probval[3]={0};
  int iJetPtBin=h2DProbLookup[0]->GetYaxis()->FindBin(jetpt);;
  int iIPBin[3]={0};

  for(int iN=0;iN<3;iN++){
    if(!hasIPs[iN])continue;
    if(ipval[iN]<0) continue;
    iIPBin[iN]=h2DProbLookup[iN]->GetXaxis()->FindBin(-ipval[iN]);
    probval[iN]=IntegrateIP(iJetPtBin,iIPBin[iN], iN);
    //probval[iN]=h2DProbLookup[iN]->GetBinContent(iIPBin[iN],iJetPtBin);
    //printf("iN=%i, iIPBin=%i, ipval=%f, lowerIP=%f, higherIP=%f, || iJetPtBin=%i, jetpt=%f, lowerjetpt=%f, higherjetpt=%f, prob=%f\n", iN, iIPBin[iN],ipval[iN],h2DProbLookup[iN]->GetXaxis()->GetBinLowEdge(iIPBin[iN]),
    //        h2DProbLookup[iN]->GetXaxis()->GetBinLowEdge(iIPBin[iN]+1), iJetPtBin,jetpt, h2DProbLookup[iN]->GetYaxis()->GetBinLowEdge(iJetPtBin),  h2DProbLookup[iN]->GetYaxis()->GetBinLowEdge(iJetPtBin+1),probval[iN]);
    prob=prob*probval[iN];
  }

  double prob3=prob-prob*TMath::Log(prob)+prob*(TMath::Log(prob)*TMath::Log(prob))*0.5;
  double prob2=prob-prob*TMath::Log(prob);
  double prob1=prob;


  if(hasIPs[2]&&ipval[2]>0){
      //printf("3 tracks with prob1=%f, prob2=%f, prob3=%f, prob=%f, prob3=%f\n",probval[0],probval[1],probval[2],prob,prob3);
      return prob3;
  }
  if(hasIPs[1]&&ipval[1]>0){
      //printf("2 tracks with prob1=%f, prob2=%f prob=%f, prob2=%f\n",probval[0],probval[1],prob,prob2);
      return prob2;
  }
  if(hasIPs[0]&&ipval[0]>0){
      //printf("1 track with prob1=%f, prob=%f, prob1=%f\n",probval[0],prob,prob1);
      return prob1;
  }
  return 0;
}

void AliAnalysisTaskHFJetIPQA::FillProbabilityHists(double jetpt,double probval,int jetflavour){
  //  printf("Filling iflavou=%i, jetpt=%f, probval=%f into histogram\n", jetflavour,jetpt,probval);
  const char * flavour[6]  = {"Unid","udsg","c","b","s",""};
  double lnprobval=-TMath::Log(probval);

  //printf("logval=%f\n",lnprobval);

  FillHist(Form("h2DProbDists%s",flavour[jetflavour]),probval,jetpt,1);     //*this->fXsectionWeightingFactor );
  FillHist(Form("h2DProbDists"),probval,jetpt,1);     //*this->fXsectionWeightingFactor );

  FillHist(Form("h2DLNProbDists%s",flavour[jetflavour]),lnprobval,jetpt,1);     //*this->fXsectionWeightingFactor );
  FillHist(Form("h2DLNProbDists"),lnprobval,jetpt,1);     //*this->fXsectionWeightingFactor );
}

void AliAnalysisTaskHFJetIPQA::Terminate(Option_t *){

    printf("\n*********************************\n");
    printf("Corrections:\n");
    printf("    MC Corrections (Data/MC+Fluka):%i\n",fDoMCCorrection);
    printf("    Track Smearing:%i\n",fRunSmearing );
    printf("    Underlying Event Subtraction:%i\n", fDoUnderlyingEventSub);
    printf("*********************************\n");
}
