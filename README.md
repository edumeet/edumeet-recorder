**installation**

- build the image:  
`docker build -t edumeet-recorder .`

- run the image (`--network=host` will be removed soon):  
`docker run -v /tmp:/records --rm -it --network=host edumeet-recorder`


**recorder-service API:**

- get status:  
`curl -i -H "Accept: application/json" -H "Content-Type: application/json" -X GET http://127.0.0.1:34568/api/status`  
return:  
`[{"id":1,"is_running":true, "records":["/tmp/recorder_service/records/2020-09-04_13:19:15__0.webm"]}, {"id":2,"is_running":true}, {"id":3,"is_running":false}]`


- start raw rtp recordings:  
`curl -i -H "Accept: application/json" -H "Content-Type: application/json" --data '{"type":"rtp", "data":{"rtpPort":5000,"rtcpPort":5001}}' -X POST http://127.0.0.1:34568/api/start`  
return:  
`{"status":"started"}`

- start html recordings / rtmp streaming:
`curl -i -H "Accept: application/json" -H "Content-Type: application/json" --data '{"type":"uri", "data":{"encoder":"h264","bitrate":6000,"uri":"https://videojs.github.io/autoplay-tests/plain/attr/autoplay.html","stream_uri":"rtmp://b.rtmp.youtube.com/live2/xxxx-xxxx-xxxx-xxxx-xxxx","mode":"record"}}' -X POST http://127.0.0.1:34568/api/start`  
return:  
`{"status":"started"}`  


  possible encoders value:  
  `"encoder":"h264"`  
  `"encoder":"vp8"`  

  
  possible mode value:  
  `"mode":"record"`  
  `"mode":"stream"`  
  `"mode":"both"`  
  

- stop recording:  
`curl -i -H "Accept: application/json" -H "Content-Type: application/json" --data '{"id":1}' -X POST http://127.0.0.1:34568/api/stop`  
return:  
`{"status":"stopped"}`


- get recordings:  
`curl -i -H "Accept: application/json" -H "Content-Type: application/json" --data '{"id":1}' -X GET http://127.0.0.1:34568/api/get_records`  
return:  
`["/tmp/recorder_service/records/2020-09-04_13:19:15__0.webm", "/tmp/recorder_service/records/2020-09-04_14:29:15__1.webm"]`

