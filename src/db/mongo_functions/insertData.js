function insertData (patientBSON, studyBSON, seriesBSON, instanceBSON)
{
    var patientCollection       = db.patients;
    var studyCollection         = db.studies;
    var seriesCollection        = db.series;
    var instancesCollection     = db.instances;
    
    // looking if the instances exist.
    var cursor = instancesCollection.find(
        {
            _id: instanceBSON['(0008,0018)']['value']
        },
        {
            _id:1        
        }
    );    
    if (cursor.hasNext())
        // Exist this SOP Instance UID
        return 'instace exist in DB'
        
        // This SOP Instance UID dose not exist. Checking the Series Instance UID
        cursor = db.series.find(
            {
                _id: seriesBSON['(0020,000e)']['value'], 
                                study_instance_uid: studyBSON['(0020,000d)']['value'],
                                patient_id: patientBSON['(0010,0020)']['value']
            },
            {
                _id:1
            }
            );
            if (cursor.hasNext()) // exist this series
            {
                instanceBSON['_id']                     = instanceBSON['(0008,0018)']['value'];
                instanceBSON['series_instance_uid']     = seriesBSON['(0020,000e)']['value'];
                instanceBSON['study_instance_uid']      = studyBSON['(0020,000d)']['vaue'];
                instanceBSON['patient_id']              = patientBSON['(0010,0020)']['value'];
                
                instancesCollection.insert(instanceBSON);
                seriesCollection.update(
                    {
                        _id: seriesBSON['(0020,000e)']['value']   
                    },
                    {
                        $push:
                        {
                            instances : instanceBSON['(0008,0018)']['value']   
                        }
                    }
                );
                
                return 'series updated';
            }
            
            // The series dose not exist. Checking if the study exist.
            cursor = studyCollection.find(
                {
                    _id: studyBSON['(0020,000d)']['value'],
                                          patient_id: patientBSON['(0010,0020)']['value']
                },
                {
                    _id: 1        
                }
            );
            if (cursor.hasNext()) // exist this study
            {
                // inserting instance
                instanceBSON['_id']                     =   instanceBSON['(0008,0018)']['value'];
                instanceBSON['series_instance_uid']     =   seriesBSON['(0020,000e)']['value'];
                instanceBSON['study_instance_uid']      =   studyBSON['(0020,000d)']['value'];
                instanceBSON['patient_id']              =   patientBSON['(0010,0020)']['value'];        
                instancesCollection.insert(instanceBSON);
                
                // inserting series
                seriesBSON['_id']                       =   seriesBSON['(0020,000e)']['value'];
                seriesBSON['study_instance_uid']        =   studyBSON['(0020,000d)']['value'];
                seriesBSON['instances']            =   Array(instanceBSON['_id']);
                seriesBSON['patient_id']                =   instanceBSON['patient_id'];        
                seriesCollection.insert(seriesBSON);
                
                // updating study
                studyCollection.update(
                    {
                        _id: studyBSON['(0020,000d)']['value'],
                                       patient_id: patientBSON['(0010,0020)']['value']
                    },
                    {
                        $push: 
                        {
                            series: seriesBSON['_id']
                        }
                    }
                );
                
                return 'study updated';
            }
            
            // study dose not exist. Checking if patient exist.
            cursor = patientCollection.find(
                {
                    '(0010,0010).value': patientBSON['(0010,0010)']['value'],
                    '(0010,0020).value': patientBSON['(0010,0020)']['value']
                }
            );    
            if (cursor.hasNext()) // exist this patient
            {
                // inserting instance
                instanceBSON['_id']                     =   instanceBSON['(0008,0018)']['value'];
                instanceBSON['series_instance_uid']     =   seriesBSON['(0020,000e)']['value'];
                instanceBSON['study_instance_uid']      =   studyBSON['(0020,000d)']['value'];
                instanceBSON['patient_id']              =   patientBSON['(0010,0020)']['value'];        
                instancesCollection.insert(instanceBSON);
                
                // inserting series
                seriesBSON['_id']                       =   instanceBSON['series_instance_uid'];
                seriesBSON['study_instance_uid']        =   instanceBSON['study_instance_uid'];
                seriesBSON['instances']            =   Array(instanceBSON['_id']);
                seriesBSON['patient_id']                =   instanceBSON['patient_id'];        
                seriesCollection.insert(seriesBSON);
                
                // inserting study
                studyBSON['_id']                        =   studyBSON['(0020,000d)']['value'];
                studyBSON['patient_id']                 =   instanceBSON['patient_id'];
                studyBSON['series']                =   Array(seriesBSON['_id']);
                studyBSON['patient_id']                 =   instanceBSON['patient_id'];
                studyCollection.insert(studyBSON);
                
                // updating patient
                patientCollection.update(
                    {
                        '(0010,0010).value': patientBSON['(0010,0010)']['value'],
                        '(0010,0020).value': patientBSON['(0010,0020)']['value'],
                    },
                    {
                        $push: 
                        {
                            studies: studyBSON['_id']
                        }
                    }
            );
            
            return 'patient updated';
            }    
            
            // new patient, study, series and instance
            
            // inserting patient
            if (patientBSON['(0010,0020)']['value'] == '' || 
                patientBSON['(0010,0020)']['value'] == null)
                patientBSON['(0010,0020)']['value'] = ObjecId().toString();    
            patientBSON['studies']                  =   Array(studyBSON['(0020,000d)']['value'])
            patientCollection.insert(patientBSON);
            
            // inserting instance
            instanceBSON['_id']                     =   instanceBSON['(0008,0018)']['value'];
            instanceBSON['series_instance_uid']     =   seriesBSON['(0020,000e)']['value'];
            instanceBSON['study_instance_uid']      =   studyBSON['(0020,000d)']['value'];
            instanceBSON['patient_id']              =   patientBSON['(0010,0020)']['value'];        
            instancesCollection.insert(instanceBSON);
            
            // inserting series
            seriesBSON['_id']                       =   seriesBSON['(0020,000e)']['value'];
            seriesBSON['study_instance_uid']        =   studyBSON['(0020,000d)']['value'];
            seriesBSON['instances']                 =   Array(instanceBSON['_id']);
            seriesBSON['patient_id']                =   patientBSON['(0010,0020)']['value'];
            seriesCollection.insert(seriesBSON);
            
            // inserting study
            studyBSON['_id']                        =   studyBSON['(0020,000d)']['value'];
            studyBSON['series']                =   Array(seriesBSON['_id']);
            studyBSON['patient_id']                 =   patientBSON['(0010,0020)']['value'];
            studyCollection.insert(studyBSON);
            
            return 'new instance';
}