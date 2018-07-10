void rootlogon(){
  gInterpreter->GenerateDictionary("remollGenericDetectorHit_t","remolltypes.hh");
  //gInterpreter->GenerateDictionary("remollGenericDetectorSum_t","remolltypes.hh");
  gInterpreter->GenerateDictionary("vector<remollGenericDetectorHit_t>","vector;remolltypes.hh");
  //gInterpreter->GenerateDictionary("vector<remollGenericDetectorSum_t>","vector;remolltypes.hh");
}
