  // $Id$
  //
  // Emcal Neutral Cluster analysis base task.
  //
  //



#include <TClonesArray.h>
#include <TChain.h>
#include <TList.h>
#include <TVector3.h>
#include <TLorentzVector.h>
#include <TH1D.h>
#include <TH2D.h>
#include <TH3D.h>
#include <THnSparse.h>
#include "AliAnalysisManager.h"
#include "AliCentrality.h"
#include "AliEMCALGeometry.h"
#include "AliESDEvent.h"
#include "AliAODEvent.h"
#include "AliLog.h"
#include "AliVCluster.h"
#include "AliVEventHandler.h"
#include "AliVParticle.h"
#include "AliClusterContainer.h"
#include "AliTrackContainer.h"
#include "AliVTrack.h"
#include "AliEmcalParticle.h"
#include "AliParticleContainer.h"
#include "AliAODCaloCluster.h"
#include "AliESDCaloCluster.h"
#include "AliVCaloCells.h"
#include "AliPicoTrack.h"
#include "AliAODMCParticle.h"
#include "AliAODMCHeader.h"
#include "AliEMCALRecoUtils.h"
#include "AliLog.h"
#include "TF1.h"
#include "Riostream.h"
#include "TRandom3.h"
#include "AliGenPythiaEventHeader.h"


#include "AliAnalysisTaskEMCALClusterTurnOn.h"

  /// \cond CLASSIMP
ClassImp(AliAnalysisTaskEMCALClusterTurnOn);
  /// \endcond

using std::cout;
using std::endl;
  //________________________________________________________________________
AliAnalysisTaskEMCALClusterTurnOn::AliAnalysisTaskEMCALClusterTurnOn() :
AliAnalysisTaskEmcal("AliAnalysisTaskEMCALClusterTurnOn",kTRUE),
  //fParticleCollArray(),
fAOD(0),
fVevent(0),
fEMCALRecoUtils(new AliEMCALRecoUtils),
  //fOutputList(0),
fIsoConeRadius(0.4),
fdetacut(0.025),
fdphicut(0.03),
fM02mincut(0.1),
fM02maxcut(0.3),
fQA(0),
fNDimensions(0),
fClusDimensions(0),
fIsNLMCut(kFALSE),
fNLMCut(0),
fNLMmin(0),
fTMClusterRejected(kTRUE),
fTMClusterInConeRejected(kTRUE),
fTest1(0),
fBinsPt(),
fBinsPtCl(),
fBinsRejection(),
fBinsEnergy(),
fBinsEta(),
fBinsPhi(),
fBinsClEta(),
fBinsClPhi(),
fEtaPhiClus(0),
fPT(0),
fE(0),
fNLM(0),
fVz(0),
fEvents(0),
fPtaftTime(0),
fCelldist(0),
fPtaftCell(0),
fNLMdist(0),
fPtaftNLM(0),
fPtaftTM(0),
fDTBC(0),
fPtaftDTBC(0),
fPtaftFC(0),
fClusTime(0),
fOutputTHnS(0),
fOutTHnS_Clust(0)
{

    // Default constructor.

    //fParticleCollArray.SetOwner(kTRUE);
    // for(Int_t i = 0; i < 12;    i++)  fGeomMatrix[i] =  0;

  SetMakeGeneralHistograms(kTRUE);
}

  //________________________________________________________________________
AliAnalysisTaskEMCALClusterTurnOn::AliAnalysisTaskEMCALClusterTurnOn(const char *name, Bool_t histo) :
AliAnalysisTaskEmcal(name, histo),
  //fParticleCollArray(),
fAOD(0),
fVevent(0),
fEMCALRecoUtils(new AliEMCALRecoUtils),
fIsoConeRadius(0.4),
fdetacut(0.025),
fdphicut(0.03),
fM02mincut(0.1),
fM02maxcut(0.3),
fQA(0),
fNDimensions(0),
fClusDimensions(0),
fIsNLMCut(kFALSE),
fNLMCut(0),
fNLMmin(0),
fTMClusterRejected(kTRUE),
fTMClusterInConeRejected(kTRUE),
fTest1(0),
fBinsPt(),
fBinsPtCl(),
fBinsRejection(),
fBinsEnergy(),
fBinsEta(),
fBinsPhi(),
fBinsClEta(),
fBinsClPhi(),
fEtaPhiClus(0),
fPT(0),
fE(0),
fNLM(0),
fVz(0),
fEvents(0),
fPtaftTime(0),
fCelldist(0),
fPtaftCell(0),
fNLMdist(0),
fPtaftNLM(0),
fPtaftTM(0),
fDTBC(0),
fPtaftDTBC(0),
fPtaftFC(0),
fClusTime(0),
fOutputTHnS(0),
fOutTHnS_Clust(0)
{

    // Standard constructor.

    //fParticleCollArray.SetOwner(kTRUE);
    //    for(Int_t i = 0; i < 12;    i++)  fGeomMatrix[i] =  0;

  SetMakeGeneralHistograms(kTRUE);
}

  //________________________________________________________________________
AliAnalysisTaskEMCALClusterTurnOn::~AliAnalysisTaskEMCALClusterTurnOn(){
    // Destructor
}


  //________________________________________________________________________
void AliAnalysisTaskEMCALClusterTurnOn::UserCreateOutputObjects(){
    // Create ouput histograms and THnSparse and TTree

  AliAnalysisTaskEmcal::UserCreateOutputObjects();
    //printf("Up here all good");


  TString sIsoMethod="\0",sBoundaries="\0";

  sIsoMethod = "Clust";

  sBoundaries = "EMCAL Acceptance";

  fOutput = new AliEmcalList(); // RH: Leak? fOutput already exists in base class
  fOutput->SetOwner();
    //Initialize the common Output histograms
  TString sTitle;



  Int_t binPT = fBinsPt.size()-1;
  Int_t binPTCl = fBinsPtCl.size()-1;
  Int_t binRejection = fBinsRejection.size()-1;
  Int_t binEnergy = fBinsEnergy.size()-1;
  Int_t binetacell = fBinsEta.size()-1;
  Int_t binphicell = fBinsPhi.size()-1;
  Int_t binetacl = fBinsClEta.size()-1;
  Int_t binphicl = fBinsClPhi.size()-1;


  Int_t bins[] = {binPT, binRejection, binEnergy, binetacell, binphicell};


          fNDimensions = sizeof(bins)/sizeof(Int_t);
  const Int_t ndims =   fNDimensions;

  sTitle = Form("Direct Photons: p_{T} ,Reason for rejection, Cell Energy, #eta distr,#phi distr; p_{T} (GeV/c); Rejection ; E (GeV) ; #eta cell index; #phi cell index");

  fOutputTHnS =  new THnSparseF("fHnOutput",sTitle.Data(), ndims, bins);
  fOutputTHnS->SetBinEdges(0,fBinsPt.data());
  fOutputTHnS->SetBinEdges(1,fBinsRejection.data());
  fOutputTHnS->SetBinEdges(2,fBinsEnergy.data());
  fOutputTHnS->SetBinEdges(3,fBinsEta.data());
  fOutputTHnS->SetBinEdges(4,fBinsPhi.data());
  fOutputTHnS->Sumw2();

  fOutput->Add(fOutputTHnS);


  Int_t binsCluster[] = {binPTCl, binRejection ,binetacl,binphicl};

  fClusDimensions = sizeof(binsCluster)/sizeof(Int_t);
  const Int_t ndimsClus = fClusDimensions;

  fOutTHnS_Clust = new THnSparseF ("fOutTHnS_Clust","E_{T}, Reason for rejection, #eta_{clus}, #phi_{clus}; E_{T} (GeV/c); Rejection ; #eta_{clus}; #phi_{clus}",ndimsClus,binsCluster);
  fOutTHnS_Clust->SetBinEdges(0,fBinsPtCl.data());
  fOutTHnS_Clust->SetBinEdges(1,fBinsRejection.data());
  fOutTHnS_Clust->SetBinEdges(2,fBinsClEta.data());
  fOutTHnS_Clust->SetBinEdges(3,fBinsClPhi.data());
  fOutTHnS_Clust->Sumw2();
  fOutput->Add(fOutTHnS_Clust);


    //Common histograms QA initialization
  if(fQA){
      //Include QA plots to the OutputList //DEFINE BETTER THE BINNING AND THE AXES LIMITS
    fClusTime = new TH1D("hClusTime_NC","Time distribution for Clusters",1800,-150.,150.);
    fClusTime->Sumw2();
    fOutput->Add(fClusTime);

    fCelldist = new TH1D("hCells","Cell distribution before cell cut",100,0.,100.);
    fCelldist->Sumw2();
    fOutput->Add(fCelldist);

    fDTBC = new TH1D("hDTBCaftTM","DTBC distribution after TM",50,0.,50.);
    fDTBC->Sumw2();
    fOutput->Add(fDTBC);

    fNLMdist = new TH1D("hNLMaftCell","NLM distribution after Cell cut",10,0.,10.);
    fNLMdist->Sumw2();
    fOutput->Add(fNLMdist);

    fEtaPhiClus = new TH2D ("hEtaPhiClusActivity","",250,-0.8,0.8, 250, 1.2, 3.4);
      //  fEtaPhiClus->Sumw2();
    fOutput->Add(fEtaPhiClus);

    fPT = new TH1D("hPt_NC","P_{T} distribution for Neutral Clusters",400,0.,100.);
    fPT->Sumw2();
    fOutput->Add(fPT);


    fE = new TH1D("hE_NC","E distribution for Clusters",200,0.,100.);
    fE->Sumw2();
    fOutput->Add(fE);

    fNLM = new TH2D("hNLM_NC","NLM distribution for Neutral Clusters",10,0.,10.,100,0.,100.);
    fNLM->Sumw2();
    fOutput->Add(fNLM);

  }
    //   Initialization of all the Common THistos for the Three different outputs
  fVz = new TH1D("hVz_NC","Vertex Z distribution",100,-50.,50.);
  fVz->Sumw2();
  fOutput->Add(fVz);

  fEvents = new TH1D("hEvents_NC","Events",100,0.,100.);
  fEvents->Sumw2();
  fOutput->Add(fEvents);

  fPtaftTime = new TH1D("hPtaftTime_NC","p_{T} distribution for Clusters after cluster time cut",200,0.,100.);
  fPtaftTime->Sumw2();
  fOutput->Add(fPtaftTime);

  fPtaftCell = new TH1D("hPtaftCell_NC","p_{T} distribution for Clusters after Ncells cut",200,0.,100.);
  fPtaftCell->Sumw2();
  fOutput->Add(fPtaftCell);

  fPtaftNLM = new TH1D("hPtaftNLM_NC","p_{T} distribution for Clusters after NLM cut",200,0.,100.);
  fPtaftNLM->Sumw2();
  fOutput->Add(fPtaftNLM);

  fPtaftTM = new TH1D("hPtaftTM_NC","p_{T} distribution for Neutral Clusters",200,0.,100.);
  fPtaftTM->Sumw2();
  fOutput->Add(fPtaftTM);

  fPtaftDTBC = new TH1D("hPtaftDTBC_NC","p_{T} distribution for Neutral Clusters after DTBC cut",200,0.,100.);
  fPtaftDTBC->Sumw2();
  fOutput->Add(fPtaftDTBC);

  fPtaftFC = new TH1D("hPtaftFC_NC","p_{T} distribution for Clusters after fiducial cut",200,0.,100.);
  fPtaftFC->Sumw2();
  fOutput->Add(fPtaftFC);






  PostData(1, fOutput);
    //     //   return;
}

  //________________________________________________________________________
Double_t* AliAnalysisTaskEMCALClusterTurnOn::GenerateFixedBinArray(Int_t n, Double_t min, Double_t max) const
{
    // Generate the bin array for the ThnSparse

  Double_t *bins = new Double_t[n+1];

  Double_t binWidth = (max-min)/n;
  bins[0] = min;
  for (Int_t i = 1; i <= n; i++) {
    bins[i] = bins[i-1]+binWidth;
  }

  return bins;
}


  //________________________________________________________________________
void AliAnalysisTaskEMCALClusterTurnOn::ExecOnce()
{
    //   Init the analysis.
    //tracks for CT Matching
  AliTrackContainer *tracks = GetTrackContainer("tpconlyMatch");
    //  Printf("name of the first track container: %s", tracks->GetClassName().Data());
  if (!tracks) {
    AliError(Form("%s: This task needs a 1particle container!", GetName()));
    return;
  }
    //tracks for Isolation
  AliTrackContainer *tracksANA = GetTrackContainer("filterTracksAna");
    //  Printf("name of the second track container: %s", tracksANA->GetClassName().Data());

  if (!tracksANA) {
    AliError(Form("%s: This task needs a 2particle container!", GetName()));
    return;
  }
    //clusters
  AliClusterContainer *clusters = GetClusterContainer(0);
  if (!clusters) {
    AliError(Form("%s: This task needs a cluster container!", GetName()));
    return;
  }

    //Init the EMCAL Framework
  AliAnalysisTaskEmcal::ExecOnce();
  if (!fLocalInitialized) {

    AliError(Form("AliAnalysisTask not initialized"));
    return;
  }
}

  //______________________________________________________________________________________
Bool_t AliAnalysisTaskEMCALClusterTurnOn::Run()
{
    // Run the analysis.
    //vertex cuts
  AliTrackContainer *tracks = GetTrackContainer("tpconlyMatch");
  if(!tracks){
    Printf("Cannot find the tracks for CT Matching");
    return kFALSE;
  }

  if(tracks->GetTrackFilterType()!= AliEmcalTrackSelection::kTPCOnlyTracks){
    AliWarning(Form("CT matching NOT performed with TPCOnly Tracks"));
    AliWarning(Form("You better be sure of what you are doing"));
  }

  AliTrackContainer *tracksANA = GetTrackContainer("filterTracksAna");
  if(!tracksANA){
    Printf("Cannot find the tracks for Isolation");
    return kFALSE;
  }

    //  Printf("FilterType of the tracks for Analysis: %d \t(should be %d)", tracksANA->GetTrackFilterType(),AliEmcalTrackSelection::kHybridTracks);

    //    AliError(Form("\n\n\n\nGO CHECK the Settings!!!! Is Isolation calculated with filteredTracks?\n\n\n\n"));
  if(tracksANA->GetTrackFilterType()!= AliEmcalTrackSelection::kHybridTracks){
    AliWarning(Form("Isolation NOT calculated with HybridTracks"));
    AliWarning(Form("You better be sure of what you are doing"));
  }


  fVevent = dynamic_cast<AliVEvent*>(InputEvent());

    //  Printf("Vertex Z coordinate for M2: %f", fVertex[2]);
    //  Printf("Vertex Z coordinate for NF: %lf", fVertex[2]);

  if (fVertex[2]>10. || fVertex[2]<-10.) return kFALSE;
    //  AliError(Form("La task tourne bien"));

  AliClusterContainer* clusters = GetClusterContainer(0);
    ////Printf("Inside Run Method, Number of clusters for this event: %d",clusters->GetNAcceptedClusters());
  Int_t nbTracksEvent;
  nbTracksEvent =InputEvent()->GetNumberOfTracks();


    // Fill events number histogram
  fEvents->Fill(0);
    //AliError(Form("one event"));


    //Fill Vertex Z histogram
  fVz->Fill(fVertex[2]);

    // delete output USEFUL LATER FOR CONTAINER CREATION !!
    //fOutClusters->Delete();
  Int_t index=0;


      //get the entries of the Cluster Container
      //whatever is a RETURN in LCAnalysis here is a CONTINUE,
      //since there are more than 1 Cluster per Event

      //    Printf("Starting loop on clusters in RUN method");

  for (auto it : clusters->accepted()){
    AliVCluster *coi = static_cast<AliVCluster*>(it);
    if(!coi) {
      AliError("No cluster found");
      return kFALSE;
    }
      //

    index=coi->GetID();
    TLorentzVector vecCOI;
    coi->GetMomentum(vecCOI,fVertex);
    Double_t coiTOF = coi->GetTOF()*1e9;
    Double_t coiM02 = coi->GetM02();

    fEtaPhiClus->Fill(vecCOI.Eta(),vecCOI.Phi());

    fPT->Fill(vecCOI.Pt());
    fE->Fill(vecCOI.E());

    Double_t checktof = coi->GetTOF()*1e9;
    fClusTime->Fill(checktof);

    if(coiTOF< -30. || coiTOF > 30.){
      FillTHnSparse(coi,vecCOI, 1.5);
      continue;
    }
    fPtaftTime->Fill(vecCOI.Pt());
    if(fQA)
      {
      fCelldist->Fill(coi->GetNCells());
      }

    if((coi->GetNCells() < 2)){
      FillTHnSparse(coi,vecCOI, 2.5);
      continue;
    }
    fPtaftCell->Fill(vecCOI.Pt());

    Int_t nlm=0;
    AliVCaloCells * fCaloCells =InputEvent()->GetEMCALCells();
    if(fCaloCells)
    {
      nlm = GetNLM(coi,fCaloCells);
      AliDebug(1,Form("NLM = %d",nlm));

      if(fQA){
        fNLMdist->Fill(nlm);
      }

      if(fIsNLMCut && fNLMCut>0 && fNLMmin>0)
        if(nlm > fNLMCut || nlm < fNLMmin ){
          FillTHnSparse(coi,vecCOI, 3.5);
          continue;
        }
    }
    else
      AliDebug(1,Form("Can't retrieve EMCAL cells"));

    fPtaftNLM->Fill(vecCOI.Pt());
    if(fTMClusterRejected)
    {
      if(ClustTrackMatching(coi)){
        FillTHnSparse(coi,vecCOI, 4.5);
        continue;
      }
    }
    fPtaftTM->Fill(vecCOI.Pt());
    if(fQA)
    {
      fDTBC->Fill(coi->GetDistanceToBadChannel());
    }

    if((coi->GetDistanceToBadChannel() < 2)){
      FillTHnSparse(coi,vecCOI, 5.5);
      continue;
    }

    fPtaftDTBC->Fill(vecCOI.Pt());

    if(coi->E()>=5. && coi->E()<70. && fQA)
      fNLM->Fill(nlm,coi->E());

    if(!CheckBoundaries(vecCOI)){
      FillTHnSparse(coi,vecCOI, 6.5);
      continue;
    }

    fPtaftFC->Fill(vecCOI.Pt());
    if(fQA){
    }
    FillTHnSparse(coi,vecCOI, 7.5);

    if(vecCOI.Pt()<5.) continue;

  }

  return kTRUE;
}

  //____________________________________________________________________
void  AliAnalysisTaskEMCALClusterTurnOn::FillTHnSparse(AliVCluster *coi,TLorentzVector vecCOI, Double_t RejectedAt){

   const Int_t ndims =   fNDimensions;
   const Int_t ndimsClus = fClusDimensions;
   Double_t outputValues[ndims];
   Double_t outputValues_clus[ndimsClus];
   AliEMCALGeometry* geom = AliEMCALGeometry::GetInstance();
   Int_t nSupMod, nModule, nIphi, nIeta, iphi, ieta;
   Int_t c_eta = 0;
   Int_t c_phi = 0;
   outputValues[0] = coi->E();
   outputValues[1] = RejectedAt;
   outputValues_clus[0] = vecCOI.Et();
   outputValues_clus[1] = RejectedAt;
   outputValues_clus[2] = vecCOI.Eta();
   outputValues_clus[3] = vecCOI.Phi();
   fOutTHnS_Clust->Fill(outputValues_clus);

   AliVCaloCells * fCaloCells =InputEvent()->GetEMCALCells();

   Int_t cellnumber = coi->GetNCells();
   Int_t IDs[cellnumber];
   Float_t energy = 0;
   for (Int_t cellcounter = 0; cellcounter<cellnumber; cellcounter++)
   {
     IDs[cellcounter] = coi->GetCellsAbsId()[cellcounter];
     geom->GetCellIndex(IDs[cellcounter],nSupMod, nModule, nIphi, nIeta);
     geom->GetCellPhiEtaIndexInSModule(nSupMod, nModule, nIphi, nIeta, iphi, ieta);
     if(nSupMod%2 == 0) c_eta = 1;
     c_phi = nSupMod/2;
     outputValues[3] = ieta + c_eta*48;
     outputValues[4] = iphi + c_phi*24;
//     else cout << "SuperModul Nummer: " << nSupMod << endl;
     energy = fCaloCells->GetCellAmplitude(IDs[cellcounter]);
     outputValues[2] = energy;
     fOutputTHnS -> Fill(outputValues);

   }
}
  //___________________________________________________________________________________
Bool_t AliAnalysisTaskEMCALClusterTurnOn::ClustTrackMatching(AliVCluster *clust) {
    // Check if the cluster match to a track

  AliTrackContainer* tracks = GetTrackContainer(0);
  AliVTrack* mt = 0;
  TLorentzVector vecClust;
  clust->GetMomentum(vecClust,fVertex);

  Int_t nbMObj = clust -> GetNTracksMatched();
  if(tracks->GetTrackFilterType()!=AliEmcalTrackSelection::kTPCOnlyTracks)  AliError(Form("NO TPC only tracks"));

  Double_t distCT=0.;

  if (nbMObj == 0) return kFALSE;

  for(Int_t i=0;i<nbMObj;i++){

    if (fIsEsd) {
      Int_t imt = clust->GetTrackMatchedIndex(0);
      if (imt >= 0) mt = static_cast<AliVTrack*>(tracks->GetAcceptParticle(imt));
    }
    else {
      mt = static_cast<AliVTrack*>(clust->GetTrackMatched(i));
      UInt_t rejectionReason = 0;
      if (!tracks->AcceptParticle(mt, rejectionReason)) mt = 0;
    }
      //  Int_t imt = partC->GetMatchedObjId(i);

    if(!mt) continue;

      //    printf("Cluster ID %d matched with track ID %d with pT %.3f",clust->GetID(),mt->GetID(),mt->Pt());


    Double_t deta = 999;
    Double_t dphi = 999;

    Double_t veta = mt->GetTrackEtaOnEMCal();
    Double_t vphi = mt->GetTrackPhiOnEMCal();

    Float_t pos[3] = {0};
    clust->GetPosition(pos);
    TVector3 cpos(pos);
    Double_t ceta     = cpos.Eta();
    Double_t cphi     = cpos.Phi();
    deta=veta-ceta;
    dphi=TVector2::Phi_mpi_pi(vphi-cphi);
      //    printf("distant deta %.3f and dphi %.3f from the cluster",deta, dphi);
    distCT=TMath::Sqrt(deta*deta+dphi*dphi);

    if(TMath::Abs(dphi)<fdphicut && TMath::Abs(deta)<fdetacut){
      return kTRUE;
    }
  }
  return kFALSE;
}

  //_____________________________________________________________________________________________
Int_t AliAnalysisTaskEMCALClusterTurnOn::GetNLM(AliVCluster *coi, AliVCaloCells* cells){
    // find the number of local maxima of a cluster adapted from AliCalorimeterUtils

  const Int_t   nc = coi->GetNCells();

  Int_t   absIdList[nc];
  Float_t maxEList[nc];

  Int_t nMax = GetNLM(coi, cells, absIdList, maxEList);

  return nMax;
}

  //_____________________________________________________________________________________________________________________________
Int_t AliAnalysisTaskEMCALClusterTurnOn::GetNLM(AliVCluster* coi, AliVCaloCells* cells, Int_t *absIdList, Float_t *maxEList) {
    // find the cluster number of local maxima adapted from AliCalorimeterUtils

  Int_t iDigitN = 0 ;
  Int_t iDigit  = 0 ;
  Int_t absId1 = -1 ;
  Int_t absId2 = -1 ;
  const Int_t nCells = coi->GetNCells();

    // Printf("Cluster Energy Before Recalculation: %.4f",coi->E());
  Float_t eCluster = RecalEnClust(coi, cells);// recalculate cluster energy, avoid non lin correction.
  Float_t localMaxCutE = 0.1;
  Float_t locMaxCutEDiff = 0.0;

  Float_t emax  = 0;
  Int_t   idmax =-1;
  for(iDigit = 0; iDigit < nCells ; iDigit++)
  {
    absIdList[iDigit] = coi->GetCellsAbsId()[iDigit]  ;
    Float_t en = cells->GetCellAmplitude(absIdList[iDigit]);
      //    Printf("Cell Energy Before Recalculation: %.4f",en);
    RecalAmpCell(en,absIdList[iDigit]);

    if( en > emax )
    {
      emax  = en ;
      idmax = absIdList[iDigit] ;
    }
  }
  for(iDigit = 0 ; iDigit < nCells; iDigit++)
  {
    if( absIdList[iDigit] >= 0 )
    {
      absId1 = coi->GetCellsAbsId()[iDigit];

      Float_t en1 = cells->GetCellAmplitude(absId1);
        //      Printf("Cell-1 Energy Before Recalculation: %.4f",en1);
      RecalAmpCell(en1,absId1);


      for(iDigitN = 0; iDigitN < nCells; iDigitN++)
      {
        absId2 = coi->GetCellsAbsId()[iDigitN] ;

        if(absId2==-1 || absId2==absId1) continue;

          //printf("\t %d : absIDj %d\n",iDigitN, absId2);

        Float_t en2 = cells->GetCellAmplitude(absId2);
          //        Printf("Cell-2 Energy Before Recalculation: %.4f",en2);
        RecalAmpCell(en2,absId2);


        if ( AreNeighbours(absId1, absId2) )
        {
            // printf("\t \t Neighbours \n");
          if ( en1 > en2 )
          {
            absIdList[iDigitN] = -1 ;
              //printf("\t \t indexN %d not local max\n",iDigitN);
              // but may be digit too is not local max ?
            if(en1 < en2 + locMaxCutEDiff) {
                //printf("\t \t index %d not local max cause locMaxCutEDiff\n",iDigit);
              absIdList[iDigit] = -1 ;
            }
          }
          else
          {
            absIdList[iDigit] = -1 ;
              //printf("\t \t index %d not local max\n",iDigitN);
              // but may be digitN too is not local max ?
            if(en1 > en2 - locMaxCutEDiff)
            {
              absIdList[iDigitN] = -1 ;
                //printf("\t \t indexN %d not local max cause locMaxCutEDiff\n",iDigit);
            }
          }
        } // if Are neighbours
          //else printf("\t \t NOT Neighbours \n");
      } // while digitN
    } // slot not empty
  } // while digit

  iDigitN = 0 ;
  for(iDigit = 0; iDigit < nCells; iDigit++)
  {
    if( absIdList[iDigit] >= 0 )
    {
      absIdList[iDigitN] = absIdList[iDigit] ;

      Float_t en = cells->GetCellAmplitude(absIdList[iDigit]);
        // Printf("NlocMax Cell Energy Before Recalculation: %.4f",en);
      RecalAmpCell(en,absIdList[iDigit]);
        // Printf("NlocMax Cell Energy After Recalculation: %.4f",en);

        //      if(fMCECellClusFracCorrOn)
        //        en*=GetMCECellClusFracCorrection(en,eCluster)/simuTotWeight;

      if(en < localMaxCutE) continue; // Maxima only with seed energy at least

      maxEList[iDigitN] = en ;

        //printf("Local max %d, id %d, en %f\n", iDigit,absIdList[iDigitN],en);
      iDigitN++ ;
    }
  }

  if ( iDigitN == 0 ){
    AliDebug(1,Form("No local maxima found, assign highest energy cell as maxima, id %d, en cell %2.2f, en cluster %2.2f",
                    idmax,emax,coi->E()));
    iDigitN      = 1     ;
    maxEList[0]  = emax  ;
    absIdList[0] = idmax ;
  }

  AliDebug(1,Form("In coi E %2.2f (wth non lin. %2.2f), M02 %2.2f, M20 %2.2f, N maxima %d",
                  coi->E(),eCluster, coi->GetM02(),coi->GetM20(), iDigitN));
  return iDigitN ;
}

  //__________________________________________________________________________________________
Bool_t AliAnalysisTaskEMCALClusterTurnOn::AreNeighbours(Int_t absId1, Int_t absId2 ) const
{
    // check if two cells are neighbour (adapted from AliCalorimeterUtils)

  Bool_t areNeighbours = kFALSE ;

  Int_t iSupMod1 = -1, iTower1 = -1, iIphi1 = -1, iIeta1 = -1, iphi1 = -1, ieta1 = -1;
  Int_t iSupMod2 = -1, iTower2 = -1, iIphi2 = -1, iIeta2 = -1, iphi2 = -1, ieta2 = -1;

  Int_t phidiff =  0, etadiff =  0;

    //first cell
  fGeom->GetCellIndex(absId1,iSupMod1,iTower1,iIphi1,iIeta1);
  fGeom->GetCellPhiEtaIndexInSModule(iSupMod1,iTower1,iIphi1, iIeta1,iphi1,ieta1);

    // second cell
  fGeom->GetCellIndex(absId2,iSupMod2,iTower2,iIphi2,iIeta2);
  fGeom->GetCellPhiEtaIndexInSModule(iSupMod2,iTower2,iIphi2, iIeta2,iphi2,ieta2);


  if(iSupMod1!=iSupMod2){
      // In case of a shared cluster, index of SM in C side, columns start at 48 and ends at 48*2-1
      // C Side impair SM, nSupMod%2=1; A side pair SM nSupMod%2=0
    if(iSupMod1%2) ieta1+=AliEMCALGeoParams::fgkEMCALCols;
    else           ieta2+=AliEMCALGeoParams::fgkEMCALCols;
  }

  phidiff = TMath::Abs( iphi1 - iphi2 ) ;
  etadiff = TMath::Abs( ieta1 - ieta2 ) ;

    //if (( coldiff <= 1 )  && ( rowdiff <= 1 ) && (coldiff + rowdiff > 0))
  if ((etadiff + phidiff == 1 ))
    areNeighbours = kTRUE ;

  return areNeighbours;
}
  //_____________________________________________________________________________________________
  /// Recalculate cell energy if recalibration factor.
  //_____________________________________________________________________________________________
void AliAnalysisTaskEMCALClusterTurnOn::RecalAmpCell(Float_t & amp, Int_t id) const
{
  Int_t iSupMod = -1, iTower = -1, iIphi = -1, iIeta = -1, iphi = -1, ieta = -1;
  Float_t ampold=amp;
  fGeom->GetCellIndex(id,iSupMod,iTower,iIphi,iIeta);
  fGeom->GetCellPhiEtaIndexInSModule(iSupMod,iTower,iIphi, iIeta,iphi,ieta);

  amp *= fEMCALRecoUtils->GetEMCALChannelRecalibrationFactor(iSupMod,ieta,iphi);
    // if(ampold!=amp)
    //  Printf("Inside cell id %d\t\tOld Amp %.4f and recalculated amplitude: %.4f",id, ampold, amp);

}

  //__________________________________________________________________________
  /// Recalibrate the cluster energy, considering the recalibration map and the energy of the cells that compose the cluster.
  //__________________________________________________________________________
Float_t AliAnalysisTaskEMCALClusterTurnOn::RecalEnClust(AliVCluster * coi,
                                                          AliVCaloCells * cells)
{
    //Printf("Inside clust Recal");
    // Initialize some used variables
  Float_t frac  = 0., energy = 0.;

  if(cells)
  {
      //Get the cluster number of cells and list of absId, check what kind of cluster do we have.

    UShort_t * index    = coi->GetCellsAbsId() ;
    Double_t * fraction = coi->GetCellsAmplitudeFraction() ;

    Int_t ncells     = coi->GetNCells();


      // Loop on the cells, get the cell amplitude and recalibration factor, multiply and and to the new energy
    for(Int_t icell = 0; icell < ncells; icell++)
    {
      Int_t absId = index[icell];

      frac =  fraction[icell];
      if(frac < 1e-3) frac = 1; //in case of EMCAL, this is set as 0, not used.

      Float_t amp = cells->GetCellAmplitude(absId);
      RecalAmpCell(amp, absId);

        //Printf("Recalibrate cell: EMCAL, cell fraction %f, cell energy: before cal %f; after cal %f",frac,cells->GetCellAmplitude(absId),amp);

      energy += amp*frac;
    }

    AliDebug(1,Form("Energy before %f, after %f",coi->E(),energy));

  } // cells available
  else
  {
    AliFatal("Cells pointer does not exist!");
  }
    //Printf("recalculated energy: %.4f",energy);
  return energy;
}

Bool_t AliAnalysisTaskEMCALClusterTurnOn::CheckBoundaries(TLorentzVector vecCOI){
    // Check if the cone around the considered cluster is in EMCAL acceptance
    //AliInfo("Inside CheckBoundaries\n");

  Double_t minPhiBound= 0. , minEtaBound= 0., maxPhiBound= 0., maxEtaBound= 0.;
  Bool_t isINBoundaries;

    minEtaBound = -0.67+fIsoConeRadius; // ""
    maxEtaBound = 0.67-fIsoConeRadius; // ""

      minPhiBound = 1.798;
      maxPhiBound = 2.740; // normally 110° but shorter cut to avoid EMCAL border

  if(vecCOI.Eta() > maxEtaBound || vecCOI.Eta() < minEtaBound || vecCOI.Phi() > maxPhiBound || vecCOI.Phi() <minPhiBound)
    isINBoundaries=kFALSE;
  else
    isINBoundaries=kTRUE;

  return isINBoundaries;
}
