function indexing()
{
    var patient_id = {patient_id: 1};
    var patient_name = {patient_name: 1}
    var study_instance_uid = {study_instance_uid: 1};
    var series_instance_uid = {series_instance_uid: 1};
    
    var instances_list = {instances_list: 1};
    var studies_list = {studies_list: 1};
    var series_list = {series_list: 1};
    
    // ensure index for instances collection
    db.instances.ensureIndex(patient_id);
    db.instances.ensureIndex(study_instance_uid);
    db.instances.ensureIndex(series_instance_uid);
    
    // ensure index for series collection
    db.series.ensureIndex(patient_id);
    db.series.ensureIndex(study_instance_uid);
    db.series.ensureIndex(series_list);
    
    // ensure index for studies collection
    db.studies.ensureIndex(patient_id);
    db.studies.ensureIndex(series_list);
    
    // ensure index for patients
    db.patients.ensureIndex(patient_id);
    db.patients.ensureIndex(patient_name);
    db.patients.ensureIndex(studies_list);
}