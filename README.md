**recorder-service API:**


- get status:  
`curl -i -H "Accept: application/json" -H "Content-Type: application/json" -X GET http://127.0.0.1:34568/api/status`  
return:  
`[{"id":1,"is_running":true}, {"id":2,"is_running":true}, {"id":3,"is_running":false}]`


- start raw rtp recordings:  
`curl -i -H "Accept: application/json" -H "Content-Type: application/json" --data '{"port":5000,"rtcpPort":5001}' -X POST http://127.0.0.1:34568/api/start`  
return:  
`{"status":"started"}`


- stop recording:  
`curl -i -H "Accept: application/json" -H "Content-Type: application/json" --data '{"id":1}' -X POST http://127.0.0.1:34568/api/stop`  
return:  
`{"path":"/tmp/recorder_service/records/2020-09-04_13:19:15__0.webm","status":"stop"}`