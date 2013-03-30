#ifndef UID_H
#define UID_H

#include <utils/Singleton.h>
#include <string>
#include <string.h>
#include <QHash>
#include <dcmtk/dcmdata/dcuid.h>
#include <dcmtk/dcmdata/dcxfer.h>


using namespace std;
using namespace __gnu_cxx;

class TransferSyntax :public Singleton<TransferSyntax>
{
    friend class Singleton;
    typedef enum {
        UID_ImplicitVREndian,
        UID_ExplicitVRLittleEndianXFer,
        UID_DeflatedExplicitVRBigEndian,
        UID_ExplicitVRBigEndian,
        UID_JPEGBaseline,
        UID_JPEGProcess2_4,
        UID_JPEGProcess3_5,
        UID_JPEGProcess6_8,
        UID_JPEGProcess7_9,
        UID_JPEGProcess10_12,
        UID_JPEGProcess11_13,
        UID_JPEGProcess14,
        UID_JPEGProcess15,
        UID_JPEGProcess16_18,
        UID_JPEGProcess17_19,
        UID_JPEGProcess20_22,
        UID_JPEGProcess21_23,
        UID_JPEGProcess24_26,
        UID_JPEGProcess25_27,
        UID_JPEGProcess28,
        UID_JPEGProcess29,
        UID_JPEGLossless,
        UID_JPEGLSLossless,
        UID_JPEGLSLossy,
        UID_JPEG2000LosslessOnly,
        UID_JPEG2000,
        UID_JPEG2000Part2MulticomponentImageCompressionLosslessOnly,
        UID_JPEG2000Part2MulticomponentImageCompression,
        UID_JPIPReferenced,
        UID_JPIPReferencedDeflate,
        UID_RLELossless,
        UID_RFC2557MIMEEncapsulation,
        UID_MPEG2MainProfileMainLevel,
        UID_MPEG4AVCH264HighProfile_Level4_1,
        UID_MPEG4AVCH264BDCompatibleHighProfile_Level4_1
    } TransferSyntaxUID;
private:
    QHash<TransferSyntaxUID, string> m_tsList;
    char** m_dcmtkXFerList;
    TransferSyntax();
    
public:
    char** dcmtkXFerList();
    inline int count(){return this->m_tsList.size();}
    string operator[](TransferSyntaxUID tsUID);
    bool isValidXFerUID(const char* uid);
    virtual ~TransferSyntax();
};


class SOPClass : public Singleton<SOPClass>
{
    friend class Singleton;
public:
    typedef enum {
        UID_VerificationSOPClassUID,
        UID_StorageCommitmentPushModelSOPClassUID,
        UID_MediaStorageDirectoryStorageUID,
        UID_ProceduralEventLoggingSOPClassUID,
        UID_ModalityPerformedProcedureStepSOPClassUID,
        UID_ModalityPerformedProcedureStepRetrieveSOPClassUID,
        UID_ModalityPerformedProcedureStepNotificationSOPClassUID,
        UID_StorageServiceClassUID,
        UID_BasicFilmSessionSOPClassUID,
        UID_PrintJobSOPClassUID,
        UID_BasicAnnotationBoxSOPClassUID,
        UID_PrinterSOPClassUID,
        UID_PrinterConfigurationRetrievalSOPClassUID,
        UID_BasicColorPrintManagementMetaSOPClassUID,
        UID_BasicFilmBoxSOPClassUID,
        UID_VOILUTBoxSOPClassUID,
        UID_PresentationLUTSOPClassUID,
        UID_MediaCreationManagementSOPClassUID,
        UID_BasicGrayscaleImageBoxSOPClassUID,
        UID_BasicColorImageBoxSOPClassUID,
        UID_BasicGrayscalePrintManagementMetaSOPClassUID,
        UID_ComputedRadiographyImageStorageUID,
        UID_DigitalXRayImageStorageForPresentationUID,
        UID_DigitalXRayImageStorageForProcessingUID,
        UID_DigitalMammographyXRayImageStorageForPresentationUID,
        UID_DigitalMammographyXRayImageStorageForProcessingUID,
        UID_DigitalIntraOralXRayImageStorageForPresentationUID,
        UID_DigitalIntraOralXRayImageStorageForProcessingUID,
        UID_EncapsulatedPDFStorageUID,
        UID_GrayscaleSoftcopyPresentationStateStorageUID,
        UID_ColorSoftcopyPresentationStateStorageUID,
        UID_PseudoColorSoftcopyPresentationStateStorageUID,
        UID_BlendingSoftcopyPresentationStateStorageUID,
        UID_XRayAngiographicImageStorageUID,
        UID_EnhancedXAImageStorageUID,
        UID_XRayRadiofluoroscopicImageStorageUID,
        UID_EnhancedXRFImageStorageUID,
        UID_CTImageStorageUID,
        UID_EnhancedCTImageStorageUID,
        UID_NuclearMedicineImageStorageUID,
        UID_UltrasoundMultiframeImageStorageUID,
        UID_MRImageStorageUID,
        UID_EnhancedMRImageStorageUID,
        UID_MRSpectroscopyStorageUID,
        UID_RadiationTherapyImageStorageUID,
        UID_RadiationTherapyDoseStorageUID,
        UID_RadiationTherapyStructureSetStorageUID,
        UID_RadiationTherapyBeamsTreatmentRecordStorageUID,
        UID_RadiationTherapyPlanStorageUID,
        UID_RadiationTherapyBrachyTreatmentRecordStorageUID,
        UID_RadiationTherapyTreatmentSummaryRecordStorageUID,
        UID_RadiationTherapyIonPlanStorageUID,
        UID_RadiationTherapyIonBeamsTreatmentRecordStorageUID,
        UID_UltrasoundImageStorageUID,
        UID_RawDataStorageUID,
        UID_SpatialRegistrationStorageUID,
        UID_SpatialFiducialsStorageUID,
        UID_DeformableSpatialRegistrationStorageUID,
        UID_SegmentationStorageUID,
        UID_RealWorldValueMappingStorageUID,
        UID_SecondaryCaptureImageStorageUID,
        UID_MultiframeSingleBitSecondaryCaptureImageStorageUID,
        UID_MultiframeGrayscaleByteSecondaryCaptureImageStorageUID,
        UID_MultiframeGrayscaleWordSecondaryCaptureImageStorageUID,
        UID_MultiframeTrueColorSecondaryCaptureImageStorageUID,
        UID_VLEndoscopicImageStorageUID,
        UID_VideoEndoscopicImageStorageUID,
        UID_VLMicroscopicImageStorageUID,
        UID_VideoMicroscopicImageStorageUID,
        UID_VLSlideCoordinatesMicroscopicImageStorageUID,
        UID_VLPhotographicImageStorageUID,
        UID_VideoPhotographicImageStorageUID,
        UID_OphthalmicPhotography8BitImageStorageUID,
        UID_OphthalmicPhotography16BitImageStorageUID,
        UID_StereometricRelationshipStorageUID,
        UID_BasicTextSRStorageUID,
        UID_EnhancedSRStorageUID,
        UID_ComprehensiveSRStorageUID,
        UID_ProcedureLogStorageUID,
        UID_MammographyCADSRStorageUID,
        UID_KeyObjectSelectionDocumentStorageUID,
        UID_ChestCADSRStorageUID,
        UID_XRayRadiationDoseSRStorageUID,
        UID_TwelveLeadECGWaveformStorageUID,
        UID_GeneralECGWaveformStorageUID,
        UID_AmbulatoryECGWaveformStorageUID,
        UID_HemodynamicWaveformStorageUID,
        UID_CardiacElectrophysiologyWaveformStorageUID,
        UID_BasicVoiceAudioWaveformStorageUID,
        UID_FINDPatientRootQueryRetrieveInformationModelUID,
        UID_MOVEPatientRootQueryRetrieveInformationModelUID,
        UID_GETPatientRootQueryRetrieveInformationModelUID,
        UID_FINDStudyRootQueryRetrieveInformationModelUID,
        UID_MOVEStudyRootQueryRetrieveInformationModelUID,
        UID_GETStudyRootQueryRetrieveInformationModelUID,
        UID_FINDModalityWorklistInformationModelUID,
        UID_GeneralPurposeWorklistManagementMetaSOPClassUID,
        UID_FINDGeneralPurposeWorklistInformationModelUID,
        UID_GeneralPurposeScheduledProcedureStepSOPClassUID,
        UID_GeneralPurposePerformedProcedureStepSOPClassUID,
        UID_InstanceAvailabilityNotificationSOPClassUID,
        UID_GeneralRelevantPatientInformationQueryUID,
        UID_BreastImagingRelevantPatientInformationQueryUID,
        UID_CardiacRelevantPatientInformationQueryUID,
        UID_HangingProtocolStorageUID,
        UID_FINDHangingProtocolInformationModelUID,
        UID_MOVEHangingProtocolInformationModelUID
    } SOPClassUID;

protected:
    QHash<SOPClassUID, string>  m_sopClassList;
    char** m_sopClassUIDList;
    SOPClass();

public:
    string operator[](SOPClassUID sopClassUID);
    char** sopClassUIDList();
    inline int count(){return this->m_sopClassList.size();}
    bool isValidSOPClassUID(const char* uid);
    virtual ~SOPClass();
};

#endif // UID_H
