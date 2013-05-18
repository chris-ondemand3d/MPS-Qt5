#include <QApplication>
#include <dicom/net/DcmAET.h>
#include <dicom/net/DcmNetSCU.h>
#include <utils/filesystem/FilesManager.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#include <dcmtk/dcmnet/dimse.h>
#include <dcmtk/dcmnet/assoc.h>
#include <dcmtk/dcmdata/dcvrui.h>
#include <dcmtk/dcmdata/dcvris.h>
#include <dicom/net/DcmNetSCP.h>
#include <dcmtk/dcmdata/dcostrma.h>
#include <dcmtk/dcmdata/dcostrma.h>
#include <dcmtk/dcmdata/dcdeftag.h>
#include <stdio.h>
#include <tasks/tasks.h>
#include <db/DBManager.h>


using namespace std;

int main(int argc, char** argv)
{
//     DBManager manager("mpsdb","localhost");
//     DcmFileFormat f;
//     f.loadFile("/home/freddy/XA.dcm");
//     manager.store(f.getDataset());
//     exit(0);
    
    QApplication app(argc, argv);
    
//     DcmAET server("COMMON", "localhost", 1111);
    DcmAET server("DCM4CHEE", "localhost", 11112);
    DcmAET aet("IMAGIS", "localhost", 55555);

    DcmNetSCU scu(aet);
//     QString msg;
//     QList<QString> files;
//     files.append("/home/freddy/images/11");
//     files.append("/home/freddy/images/12");
//     files.append("/home/freddy/images/13");
//     files.append("/home/freddy/images/14");
//     QString filename("/home/freddy/images/11.dcm");
//     FileManager fm("/media/Trabajo/dicom/1/");
//     
// //     Status status = scu.cstore_RQ(server, filename);
    DcmQuery query;
    DcmUniqueIdentifier* studyInstanceUID = new DcmUniqueIdentifier(DCM_StudyInstanceUID);
    studyInstanceUID->putString("1.3.12.2.1107.5.1.4.54181.30000007070814211218700000055");
//     DcmUniqueIdentifier* seriesInstanceUID = new DcmUniqueIdentifier(DCM_SeriesInstanceUID);
//     seriesInstanceUID->putString("1.3.12.2.1107.5.4.2.6226.20040721.161218.167");    
//     DcmUniqueIdentifier* sopInstanceUID = new DcmUniqueIdentifier(DCM_SOPInstanceUID);
//     sopInstanceUID->putString("1.3.12.2.1107.5.4.3.33239335544320.20040721.153850.662");
    
//     DcmUniqueIdentifier* studyInstanceUID = new DcmUniqueIdentifier(DCM_StudyInstanceUID);
//     studyInstanceUID->putString("1.3.12.2.1107.5.1.4.54181.30000007070814211218700000055");
//     DcmUniqueIdentifier* seriesInstanceUID = new DcmUniqueIdentifier(DCM_SeriesInstanceUID);
//     seriesInstanceUID->putString("1.3.12.2.1107.5.1.4.54181.30000007070814230000000007368");    
//     DcmUniqueIdentifier* sopInstanceUID = new DcmUniqueIdentifier(DCM_SOPInstanceUID);
//     sopInstanceUID->putString("1.3.12.2.1107.5.1.4.54181.30000007070814230000000007536");
    
    query.setQueryLevel(DcmQuery::STUDY_LEVEL);
    
    DcmIntegerString* numberOfInstace = new DcmIntegerString(DCM_NumberOfPatientRelatedInstances);
    query.addKey(studyInstanceUID);
//     query.addKey(seriesInstanceUID);
//     query.addKey(sopInstanceUID);
    query.addKey(numberOfInstace);
    TaskFactory::newDcmSCPInstance(new DcmNetSCP(aet, "/home/freddy/tmp/move/"))->start();
    for (int i = 0; i < 10000; i++);
    Status status = scu.cmove_RQ(server, scu.getAET(), query, const_cast<char*>("/home/freddy/tmp/move/"));
    
//     
//     delete studyInstanceUID;
//     delete sopInstanceUID;
// //     Status status = scu.cstore_RQ(server, fm);
// //     Status status = scu.cstore_RQ(server, filename);
// //     Status status = scu.cecho_RQ(server);
// //     Status status(Status::AssociationError, "");
//     if (status.good())
//         cout << "...ECHO SUCCESS..." << endl << "Message: " << status.message().c_str() << endl;
//     else
//         cout << "ECHO ERROR... " << endl << "Message: " << status.message().c_str() << endl;

//     FileManager dcmfile("/media/Trabajo/dicom/");
//     DcmFileFormat* file = NULL;
// 
//     OFString pname;
//     while (dcmfile.nextDcmFile(file).good())
//     {
//         file->getDataset()->findAndGetOFString(DCM_PatientName, pname);
//         cout << "patient name: " << pname.c_str() << endl;
//         if (file != NULL)
//             delete file;
//     }   
// // 
//     cout << "************************ E N D ************************";

//     T_ASC_Network *net; // network struct, contains DICOM upper layer FSM etc.
//     ASC_initializeNetwork(NET_REQUESTOR, 0, 1000 /* timeout */, &net);
// 
//     T_ASC_Parameters *params; // parameters of association request
//     ASC_createAssociationParameters(&params, ASC_DEFAULTMAXPDU);
// 
//     // set calling and called AE titles
//     ASC_setAPTitles(params, "IMAGIS", "DCM4CHEE", NULL);
// 
//     // the DICOM server accepts connections at server.nowhere.com port 104
//     ASC_setPresentationAddresses(params, "localhost", "localhost:11112");
// 
//     // list of transfer syntaxes, only a single entry here
//     const char* ts[] = { UID_LittleEndianImplicitTransferSyntax };
// 
//     // add presentation context to association request
//     ASC_addPresentationContext(params, 1, UID_VerificationSOPClass, ts, 1);
// 
//     // request DICOM association
//     T_ASC_Association *assoc;
//     if (ASC_requestAssociation(net, params, &assoc).good())
//     {
//         if (ASC_countAcceptedPresentationContexts(params) == 1)
//         {
//             // the remote SCP has accepted the Verification Service Class
//             DIC_US id = assoc->nextMsgID++; // generate next message ID
//             DIC_US status; // DIMSE status of C-ECHO-RSP will be stored here
//             DcmDataset *sd = NULL; // status detail will be stored here
//             // send C-ECHO-RQ and handle response
//             DIMSE_echoUser(assoc, id, DIMSE_BLOCKING, 0, &status, &sd);
//             delete sd; // we don't care about status detail
//         }
//     }
//     ASC_releaseAssociation(assoc); // release association
//     ASC_destroyAssociation(&assoc); // delete assoc structure
//     ASC_dropNetwork(&net); // delete net structure
//     DcmNetSCP server(DcmAET("COMMON", "localhost", 1111));
//     server.start();
    
//     cout << status.message().c_str() << endl;
    return app.exec();
}
