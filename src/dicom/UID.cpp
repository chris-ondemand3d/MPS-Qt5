#include "UID.h"
#include <iostream>
#include <boost/concept_check.hpp>

TransferSyntax::TransferSyntax()
{
    this->m_tsList[UID_ImplicitVREndian] = "1.2.840.10008.1.2";
    this->m_tsList[UID_ExplicitVRLittleEndianXFer] = "1.2.840.10008.1.2.1";
    this->m_tsList[UID_DeflatedExplicitVRBigEndian] = "1.2.840.10008.1.2.1.99";
    this->m_tsList[UID_ExplicitVRBigEndian] = "1.2.840.10008.1.2.2";
    this->m_tsList[UID_JPEGBaseline] = "1.2.840.10008.1.2.4.50";
    this->m_tsList[UID_JPEGProcess2_4] = "1.2.840.10008.1.2.4.51";
    this->m_tsList[UID_JPEGProcess3_5] = "1.2.840.10008.1.2.4.52";
    this->m_tsList[UID_JPEGProcess6_8] = "1.2.840.10008.1.2.4.53";
    this->m_tsList[UID_JPEGProcess7_9] = "1.2.840.10008.1.2.4.54";
    this->m_tsList[UID_JPEGProcess10_12] = "1.2.840.10008.1.2.4.55";
    this->m_tsList[UID_JPEGProcess11_13] = "1.2.840.10008.1.2.4.56";
    this->m_tsList[UID_JPEGProcess14] = "1.2.840.10008.1.2.4.57";
    this->m_tsList[UID_JPEGProcess15] = "1.2.840.10008.1.2.4.58";
    this->m_tsList[UID_JPEGProcess16_18] = "1.2.840.10008.1.2.4.59";
    this->m_tsList[UID_JPEGProcess17_19] = "1.2.840.10008.1.2.4.60";
    this->m_tsList[UID_JPEGProcess20_22] = "1.2.840.10008.1.2.4.61";
    this->m_tsList[UID_JPEGProcess21_23] = "1.2.840.10008.1.2.4.62";
    this->m_tsList[UID_JPEGProcess24_26] = "1.2.840.10008.1.2.4.63";
    this->m_tsList[UID_JPEGProcess25_27] = "1.2.840.10008.1.2.4.64";
    this->m_tsList[UID_JPEGProcess28] = "1.2.840.10008.1.2.4.65";
    this->m_tsList[UID_JPEGProcess29] = "1.2.840.10008.1.2.4.66";
    this->m_tsList[UID_JPEGLossless] = "1.2.840.10008.1.2.4.70";
    this->m_tsList[UID_JPEGLSLossless] = "1.2.840.10008.1.2.4.80";
    this->m_tsList[UID_JPEGLSLossy] = "1.2.840.10008.1.2.4.81";
    this->m_tsList[UID_JPEG2000LosslessOnly] = "1.2.840.10008.1.2.4.90";
    this->m_tsList[UID_JPEG2000] = "1.2.840.10008.1.2.4.91";
    this->m_tsList[UID_JPEG2000Part2MulticomponentImageCompressionLosslessOnly] = "1.2.840.10008.1.2.4.92";
    this->m_tsList[UID_JPEG2000Part2MulticomponentImageCompression] = "1.2.840.10008.1.2.4.93";
    this->m_tsList[UID_JPIPReferenced] = "1.2.840.10008.1.2.4.94";
    this->m_tsList[UID_JPIPReferencedDeflate] = "1.2.840.10008.1.2.4.95";
    this->m_tsList[UID_RLELossless] = "1.2.840.10008.1.2.5";
    this->m_tsList[UID_RFC2557MIMEEncapsulation] = "1.2.840.10008.1.2.6.1";
    this->m_tsList[UID_MPEG2MainProfileMainLevel] = "1.2.840.10008.1.2.4.100";
    this->m_tsList[UID_MPEG4AVCH264HighProfile_Level4_1] = "1.2.840.10008.1.2.4.102";
    this->m_tsList[UID_MPEG4AVCH264BDCompatibleHighProfile_Level4_1] = "1.2.840.10008.1.2.4.103";

    // Creating the DCMTK Transfer Syntax List
    int length = this->m_tsList.size();
    
    this->m_dcmtkXFerList= new char*[length];
    int i = 0;
    for (QHash<TransferSyntaxUID, string>::iterator it = this->m_tsList.begin();
         it != this->m_tsList.end(); it++, i++)
        this->m_dcmtkXFerList[i] = strdup((*it).c_str());
}

string TransferSyntax::operator[](TransferSyntax::TransferSyntaxUID tsUID)
{
    return this->m_tsList[tsUID];
}

bool TransferSyntax::isValidXFerUID(const char* uid)
{
    
}


char** TransferSyntax::dcmtkXFerList()
{
    return this->m_dcmtkXFerList;
}

TransferSyntax::~TransferSyntax()
{
    this->m_tsList.clear();
}

// *****************************************************************************
SOPClass::SOPClass()
{
    this->m_sopClassList[UID_VerificationSOPClassUID] = "1.2.840.10008.1.1";
    this->m_sopClassList[UID_StorageCommitmentPushModelSOPClassUID] = "1.2.840.10008.1.20.1";
    this->m_sopClassList[UID_MediaStorageDirectoryStorageUID] = "1.2.840.10008.1.3.10";
    this->m_sopClassList[UID_ProceduralEventLoggingSOPClassUID] = "1.2.840.10008.1.40";
    this->m_sopClassList[UID_ModalityPerformedProcedureStepSOPClassUID] = "1.2.840.10008.3.1.2.3.3";
    this->m_sopClassList[UID_ModalityPerformedProcedureStepRetrieveSOPClassUID] = "1.2.840.10008.3.1.2.3.4";
    this->m_sopClassList[UID_ModalityPerformedProcedureStepNotificationSOPClassUID] = "1.2.840.10008.3.1.2.3.5";
    this->m_sopClassList[UID_StorageServiceClassUID] = "1.2.840.10008.4.2";
    this->m_sopClassList[UID_BasicFilmSessionSOPClassUID] = "1.2.840.10008.5.1.1.1";
    this->m_sopClassList[UID_PrintJobSOPClassUID] = "1.2.840.10008.5.1.1.14";
    this->m_sopClassList[UID_BasicAnnotationBoxSOPClassUID] = "1.2.840.10008.5.1.1.15";
    this->m_sopClassList[UID_PrinterSOPClassUID] = "1.2.840.10008.5.1.1.16";
    this->m_sopClassList[UID_PrinterConfigurationRetrievalSOPClassUID] = "1.2.840.10008.5.1.1.16.376";
    this->m_sopClassList[UID_BasicColorPrintManagementMetaSOPClassUID] = "1.2.840.10008.5.1.1.18";
    this->m_sopClassList[UID_BasicFilmBoxSOPClassUID] = "1.2.840.10008.5.1.1.2";
    this->m_sopClassList[UID_VOILUTBoxSOPClassUID] = "1.2.840.10008.5.1.1.22";
    this->m_sopClassList[UID_PresentationLUTSOPClassUID] = "1.2.840.10008.5.1.1.23";
    this->m_sopClassList[UID_MediaCreationManagementSOPClassUID] = "1.2.840.10008.5.1.1.33";
    this->m_sopClassList[UID_BasicGrayscaleImageBoxSOPClassUID] = "1.2.840.10008.5.1.1.4";
    this->m_sopClassList[UID_BasicColorImageBoxSOPClassUID] = "1.2.840.10008.5.1.1.4.1";
    this->m_sopClassList[UID_BasicGrayscalePrintManagementMetaSOPClassUID] = "1.2.840.10008.5.1.1.9";
    this->m_sopClassList[UID_ComputedRadiographyImageStorageUID] = "1.2.840.10008.5.1.4.1.1.1";
    this->m_sopClassList[UID_DigitalXRayImageStorageForPresentationUID] = "1.2.840.10008.5.1.4.1.1.1.1";
    this->m_sopClassList[UID_DigitalXRayImageStorageForProcessingUID] = "1.2.840.10008.5.1.4.1.1.1.1.1";
    this->m_sopClassList[UID_DigitalMammographyXRayImageStorageForPresentationUID] = "1.2.840.10008.5.1.4.1.1.1.2";
    this->m_sopClassList[UID_DigitalMammographyXRayImageStorageForProcessingUID] = "1.2.840.10008.5.1.4.1.1.1.2.1";
    this->m_sopClassList[UID_DigitalIntraOralXRayImageStorageForPresentationUID] = "1.2.840.10008.5.1.4.1.1.1.3";
    this->m_sopClassList[UID_DigitalIntraOralXRayImageStorageForProcessingUID] = "1.2.840.10008.5.1.4.1.1.1.3.1";
    this->m_sopClassList[UID_EncapsulatedPDFStorageUID] = "1.2.840.10008.5.1.4.1.1.104.1";
    this->m_sopClassList[UID_GrayscaleSoftcopyPresentationStateStorageUID] = "1.2.840.10008.5.1.4.1.1.11.1";
    this->m_sopClassList[UID_ColorSoftcopyPresentationStateStorageUID] = "1.2.840.10008.5.1.4.1.1.11.2";
    this->m_sopClassList[UID_PseudoColorSoftcopyPresentationStateStorageUID] = "1.2.840.10008.5.1.4.1.1.11.3";
    this->m_sopClassList[UID_BlendingSoftcopyPresentationStateStorageUID] = "1.2.840.10008.5.1.4.1.1.11.4";
    this->m_sopClassList[UID_XRayAngiographicImageStorageUID] = "1.2.840.10008.5.1.4.1.1.12.1";
    this->m_sopClassList[UID_EnhancedXAImageStorageUID] = "1.2.840.10008.5.1.4.1.1.12.1.1";
    this->m_sopClassList[UID_XRayRadiofluoroscopicImageStorageUID] = "1.2.840.10008.5.1.4.1.1.12.2";
    this->m_sopClassList[UID_EnhancedXRFImageStorageUID] = "1.2.840.10008.5.1.4.1.1.12.2.1";
    this->m_sopClassList[UID_CTImageStorageUID] = "1.2.840.10008.5.1.4.1.1.2";
    this->m_sopClassList[UID_EnhancedCTImageStorageUID] = "1.2.840.10008.5.1.4.1.1.2.1";
    this->m_sopClassList[UID_NuclearMedicineImageStorageUID] = "1.2.840.10008.5.1.4.1.1.20";
    this->m_sopClassList[UID_UltrasoundMultiframeImageStorageUID] = "1.2.840.10008.5.1.4.1.1.3.1";
    this->m_sopClassList[UID_MRImageStorageUID] = "1.2.840.10008.5.1.4.1.1.4";
    this->m_sopClassList[UID_EnhancedMRImageStorageUID] = "1.2.840.10008.5.1.4.1.1.4.1";
    this->m_sopClassList[UID_MRSpectroscopyStorageUID] = "1.2.840.10008.5.1.4.1.1.4.2";
    this->m_sopClassList[UID_RadiationTherapyImageStorageUID] = "1.2.840.10008.5.1.4.1.1.481.1";
    this->m_sopClassList[UID_RadiationTherapyDoseStorageUID] = "1.2.840.10008.5.1.4.1.1.481.2";
    this->m_sopClassList[UID_RadiationTherapyStructureSetStorageUID] = "1.2.840.10008.5.1.4.1.1.481.3";
    this->m_sopClassList[UID_RadiationTherapyBeamsTreatmentRecordStorageUID] = "1.2.840.10008.5.1.4.1.1.481.4";
    this->m_sopClassList[UID_RadiationTherapyPlanStorageUID] = "1.2.840.10008.5.1.4.1.1.481.5";
    this->m_sopClassList[UID_RadiationTherapyBrachyTreatmentRecordStorageUID] = "1.2.840.10008.5.1.4.1.1.481.6";
    this->m_sopClassList[UID_RadiationTherapyTreatmentSummaryRecordStorageUID] = "1.2.840.10008.5.1.4.1.1.481.7";
    this->m_sopClassList[UID_RadiationTherapyIonPlanStorageUID] = "1.2.840.10008.5.1.4.1.1.481.8";
    this->m_sopClassList[UID_RadiationTherapyIonBeamsTreatmentRecordStorageUID] = "1.2.840.10008.5.1.4.1.1.481.9";
    this->m_sopClassList[UID_UltrasoundImageStorageUID] = "1.2.840.10008.5.1.4.1.1.6.1";
    this->m_sopClassList[UID_RawDataStorageUID] = "1.2.840.10008.5.1.4.1.1.66";
    this->m_sopClassList[UID_SpatialRegistrationStorageUID] = "1.2.840.10008.5.1.4.1.1.66.1";
    this->m_sopClassList[UID_SpatialFiducialsStorageUID] = "1.2.840.10008.5.1.4.1.1.66.2";
    this->m_sopClassList[UID_DeformableSpatialRegistrationStorageUID] = "1.2.840.10008.5.1.4.1.1.66.3";
    this->m_sopClassList[UID_SegmentationStorageUID] = "1.2.840.10008.5.1.4.1.1.66.4";
    this->m_sopClassList[UID_RealWorldValueMappingStorageUID] = "1.2.840.10008.5.1.4.1.1.67";
    this->m_sopClassList[UID_SecondaryCaptureImageStorageUID] = "1.2.840.10008.5.1.4.1.1.7";
    this->m_sopClassList[UID_MultiframeSingleBitSecondaryCaptureImageStorageUID] = "1.2.840.10008.5.1.4.1.1.7.1";
    this->m_sopClassList[UID_MultiframeGrayscaleByteSecondaryCaptureImageStorageUID] = "1.2.840.10008.5.1.4.1.1.7.2";
    this->m_sopClassList[UID_MultiframeGrayscaleWordSecondaryCaptureImageStorageUID] = "1.2.840.10008.5.1.4.1.1.7.3";
    this->m_sopClassList[UID_MultiframeTrueColorSecondaryCaptureImageStorageUID] = "1.2.840.10008.5.1.4.1.1.7.4";
    this->m_sopClassList[UID_VLEndoscopicImageStorageUID] = "1.2.840.10008.5.1.4.1.1.77.1.1";
    this->m_sopClassList[UID_VideoEndoscopicImageStorageUID] = "1.2.840.10008.5.1.4.1.1.77.1.1.1";
    this->m_sopClassList[UID_VLMicroscopicImageStorageUID] = "1.2.840.10008.5.1.4.1.1.77.1.2";
    this->m_sopClassList[UID_VideoMicroscopicImageStorageUID] = "1.2.840.10008.5.1.4.1.1.77.1.2.1";
    this->m_sopClassList[UID_VLSlideCoordinatesMicroscopicImageStorageUID] = "1.2.840.10008.5.1.4.1.1.77.1.3";
    this->m_sopClassList[UID_VLPhotographicImageStorageUID] = "1.2.840.10008.5.1.4.1.1.77.1.4";
    this->m_sopClassList[UID_VideoPhotographicImageStorageUID] = "1.2.840.10008.5.1.4.1.1.77.1.4.1";
    this->m_sopClassList[UID_OphthalmicPhotography8BitImageStorageUID] = "1.2.840.10008.5.1.4.1.1.77.1.5.1";
    this->m_sopClassList[UID_OphthalmicPhotography16BitImageStorageUID] = "1.2.840.10008.5.1.4.1.1.77.1.5.2";
    this->m_sopClassList[UID_StereometricRelationshipStorageUID] = "1.2.840.10008.5.1.4.1.1.77.1.5.3";
    this->m_sopClassList[UID_BasicTextSRStorageUID] = "1.2.840.10008.5.1.4.1.1.88.11";
    this->m_sopClassList[UID_EnhancedSRStorageUID] = "1.2.840.10008.5.1.4.1.1.88.22";
    this->m_sopClassList[UID_ComprehensiveSRStorageUID] = "1.2.840.10008.5.1.4.1.1.88.33";
    this->m_sopClassList[UID_ProcedureLogStorageUID] = "1.2.840.10008.5.1.4.1.1.88.40";
    this->m_sopClassList[UID_MammographyCADSRStorageUID] = "1.2.840.10008.5.1.4.1.1.88.50";
    this->m_sopClassList[UID_KeyObjectSelectionDocumentStorageUID] = "1.2.840.10008.5.1.4.1.1.88.59";
    this->m_sopClassList[UID_ChestCADSRStorageUID] = "1.2.840.10008.5.1.4.1.1.88.65";
    this->m_sopClassList[UID_XRayRadiationDoseSRStorageUID] = "1.2.840.10008.5.1.4.1.1.88.67";
    this->m_sopClassList[UID_TwelveLeadECGWaveformStorageUID] = "1.2.840.10008.5.1.4.1.1.9.1.1";
    this->m_sopClassList[UID_GeneralECGWaveformStorageUID] = "1.2.840.10008.5.1.4.1.1.9.1.2";
    this->m_sopClassList[UID_AmbulatoryECGWaveformStorageUID] = "1.2.840.10008.5.1.4.1.1.9.1.3";
    this->m_sopClassList[UID_HemodynamicWaveformStorageUID] = "1.2.840.10008.5.1.4.1.1.9.2.1";
    this->m_sopClassList[UID_CardiacElectrophysiologyWaveformStorageUID] = "1.2.840.10008.5.1.4.1.1.9.3.1";
    this->m_sopClassList[UID_BasicVoiceAudioWaveformStorageUID] = "1.2.840.10008.5.1.4.1.1.9.4.1";
    this->m_sopClassList[UID_FINDPatientRootQueryRetrieveInformationModelUID] = "1.2.840.10008.5.1.4.1.2.1.1";
    this->m_sopClassList[UID_MOVEPatientRootQueryRetrieveInformationModelUID] = "1.2.840.10008.5.1.4.1.2.1.2";
    this->m_sopClassList[UID_GETPatientRootQueryRetrieveInformationModelUID] = "1.2.840.10008.5.1.4.1.2.1.3";
    this->m_sopClassList[UID_FINDStudyRootQueryRetrieveInformationModelUID] = "1.2.840.10008.5.1.4.1.2.2.1";
    this->m_sopClassList[UID_MOVEStudyRootQueryRetrieveInformationModelUID] = "1.2.840.10008.5.1.4.1.2.2.2";
    this->m_sopClassList[UID_GETStudyRootQueryRetrieveInformationModelUID] = "1.2.840.10008.5.1.4.1.2.2.3";
    this->m_sopClassList[UID_FINDModalityWorklistInformationModelUID] = "1.2.840.10008.5.1.4.31";
    this->m_sopClassList[UID_GeneralPurposeWorklistManagementMetaSOPClassUID] = "1.2.840.10008.5.1.4.32";
    this->m_sopClassList[UID_FINDGeneralPurposeWorklistInformationModelUID] = "1.2.840.10008.5.1.4.32.1";
    this->m_sopClassList[UID_GeneralPurposeScheduledProcedureStepSOPClassUID] = "1.2.840.10008.5.1.4.32.2";
    this->m_sopClassList[UID_GeneralPurposePerformedProcedureStepSOPClassUID] = "1.2.840.10008.5.1.4.32.3";
    this->m_sopClassList[UID_InstanceAvailabilityNotificationSOPClassUID] = "1.2.840.10008.5.1.4.33";
    this->m_sopClassList[UID_GeneralRelevantPatientInformationQueryUID] = "1.2.840.10008.5.1.4.37.1";
    this->m_sopClassList[UID_BreastImagingRelevantPatientInformationQueryUID] = "1.2.840.10008.5.1.4.37.2";
    this->m_sopClassList[UID_CardiacRelevantPatientInformationQueryUID] = "1.2.840.10008.5.1.4.37.3";
    this->m_sopClassList[UID_HangingProtocolStorageUID] = "1.2.840.10008.5.1.4.38.1";
    this->m_sopClassList[UID_FINDHangingProtocolInformationModelUID] = "1.2.840.10008.5.1.4.38.2";
    this->m_sopClassList[UID_MOVEHangingProtocolInformationModelUID] = "1.2.840.10008.5.1.4.38.3";

    int length = this->m_sopClassList.size();
    this->m_sopClassUIDList = new char*[length];

    int i = 0;
    for (QHash<SOPClassUID, string>::iterator it = this->m_sopClassList.begin();
         it != this->m_sopClassList.end(); it++, i++)
         this->m_sopClassUIDList[i] = strdup((*it).c_str());
}

bool SOPClass::isValidSOPClassUID(const char* uid)
{

}


string SOPClass::operator[](SOPClassUID sopClassUID)
{
    return this->m_sopClassList[sopClassUID];
}

char** SOPClass::sopClassUIDList()
{
    return this->m_sopClassUIDList;
}


SOPClass::~SOPClass()
{
    for (int i = 0; i < this->m_sopClassList.count(); i++)
        delete this->m_sopClassUIDList[i];
    delete this->m_sopClassUIDList;
    
    this->m_sopClassList.clear();
}

