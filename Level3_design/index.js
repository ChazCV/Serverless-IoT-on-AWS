var AWS = require('aws-sdk');
//configuring the AWS environment
//default name for this Lambda fuction should be 'index.js'

var s3 = new AWS.S3({ apiVersion: '2006-03-01' });

exports.handler = function (event, context, callback) {

    let responseData = [];
    let promiseArr = [];
    var params = {
        Bucket: "ours3bucketsection6",
        MaxKeys: 50
    };
    s3.listObjects(params, (err, objects) => {
        if (err)
            return console.log(err);
        if (objects && objects.Contents && objects.Contents.length > 0) {
            let contents = objects.Contents;
            // console.log(contents);
            for (let index = 0; index < contents.length; index++) {
                const element = contents[index];
                const regex = /folder2*/g;  // YOUR KEY/FOLDER NAME FOLLOWED BY a '*'
                if (regex.test(element.Key)) {
                    var params1 = {
                        Bucket: "ours3bucketsection6",
                        Key: element.Key
                    };
                    if (element.Key !== 'folder2/') {
                        promiseArr.push(new Promise((resolve, reject) => {
                            s3.getObject(params1, (err, data) => {
                                if (err) {
                                    console.log(err);
                                    reject();
                                } else {
                                    try {                                        
                                        let parsedData = JSON.parse(data.Body.toString('utf-8'));
                                        if(parsedData && parsedData.hasOwnProperty('humidity') && parsedData.hasOwnProperty('temperature') && parsedData.hasOwnProperty('uptime')){
                                            responseData.push(parsedData);
                                        }
                                        resolve();
                                    } catch (error) {
                                        console.log(error)
                                        resolve();
                                    }
                                }
                            })
                        }))

                    }
                }
            }

            Promise.all(promiseArr)
                .then(res => {
                    // console.log(JSON.stringify(responseData));
                    callback(null, responseData);
                })
                .catch(err => {
                    console.log('Error ', err)
                    callback(err);
                });
        }
    })
}

