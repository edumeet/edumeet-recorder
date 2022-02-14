# eduMeet Recorder

## Getting started

For example, given that you have the following URI:
 * https://letsmeet.no/test-edumeet-recorder

Steps to run the minimal example are:

  1. `git clone https://github.com/edumeet/edumeet-recorder.git`
  2. `cd edumeet-recorder`
  3. `docker build -t edumeet-recorder .` (few minutes to half an hour building)
  4. `docker run -v /tmp:/records --rm -it --network=host edumeet-recorder`
  5. Open in browser: https://letsmeet.no/test-edumeet-recorder
  6. Use curl to start recording:
```
curl \
  -i \
  -H "Accept: application/json" \
  -H "Content-Type: application/json" \
    --data '{"type":"uri", \
                 "data":{"encoder":"h264", \
                "bitrate":6000, \
                "uri":"https://letsmeet.no/test-edumeet-recorder?headless=true&displayName=recorder", \
                "stream_uri":"","mode":"record"}}' \
  -X POST http://127.0.0.1:34570/api/start
```
  7. Use curl stop to stop recording:
```
curl \
  -i \
  -H "Accept: application/json" \
  -H "Content-Type: application/json" \
    --data '{"id":1}' \
  -X POST http://127.0.0.1:34570/api/stop
```

Notes:
 * `headless=true` -- is used to skip the login/user/landing screen for eduMeet (a temporary hack)

## More examples and explanation

### Installation

- build the image:  
`docker build -t edumeet-recorder .`

- run the image:  
`docker run -v /tmp:/records --rm -it --network=host edumeet-recorder`

### Example usage

- Session 1:

`curl -i -H "Accept: application/json" -H "Content-Type: application/json" --data '{"type":"uri", "data":{"encoder":"h264","bitrate":6000,"uri":"https://letsmeet.no/roomname?headless=true&displayName=recorder","stream_uri":"","mode":"record"}}' -X POST http://127.0.0.1:34570/api/start`

HTTP/1.1 200 OK
Content-Length: 20
Content-Type: application/json

{"status":"started"}

- Session 2:

`curl -i -H "Accept: application/json" -H "Content-Type: application/json" --data '{"type":"uri", "data":{"encoder":"h264","bitrate":6000,"uri":"https://letsmeet.no/roomname?headless=true&displayName=recorder","stream_uri":"","mode":"record"}}' -X POST http://127.0.0.1:34570/api/start`  

HTTP/1.1 200 OK
Content-Length: 20
Content-Type: application/json

{"status":"started"}

- Status:

`curl -i -H "Accept: application/json" -H "Content-Type: application/json" -X GET http://127.0.0.1:34570/api/status`  

HTTP/1.1 200 OK
Content-Length: 75
Content-Type: application/json

[{"id":1,"records":[],"running":true},{"id":2,"records":[],"running":true}]

- Stop:

`curl -i -H "Accept: application/json" -H "Content-Type: application/json" --data '{"id":1}' -X POST http://127.0.0.1:34570/api/stop`  

HTTP/1.1 200 OK
Content-Length: 20
Content-Type: application/json

{"status":"stopped"}

- Status:

`curl -i -H "Accept: application/json" -H "Content-Type: application/json" -X GET http://127.0.0.1:34570/api/status`  

HTTP/1.1 200 OK
Content-Length: 109
Content-Type: application/json

[{"id":1,"records":["/records/2021-09-22_07:56:00__0"],"running":false},{"id":2,"records":[],"running":true}]

Stop 2:

`curl -i -H "Accept: application/json" -H "Content-Type: application/json" --data '{"id":2}' -X POST http://127.0.0.1:34570/api/stop`  

HTTP/1.1 200 OK
Content-Length: 20
Content-Type: application/json

{"status":"stopped"}

- Status 2:

`curl -i -H "Accept: application/json" -H "Content-Type: application/json" -X GET http://127.0.0.1:34570/api/status`  

HTTP/1.1 200 OK
Content-Length: 143
Content-Type: application/json

[{"id":1,"records":["/records/2021-09-22_07:56:00__0"],"running":false},{"id":2,"records":["/records/2021-09-22_07:57:58__1"],"running":false}]

### Recorder-service API

- get status:  
`curl -i -H "Accept: application/json" -H "Content-Type: application/json" -X GET http://127.0.0.1:34570/api/status`  
return:  
`[{"id":1,"is_running":true, "records":["/tmp/recorder_service/records/2020-09-04_13:19:15__0.webm"]}, {"id":2,"is_running":true}, {"id":3,"is_running":false}]`


- start raw rtp recordings:  
`curl -i -H "Accept: application/json" -H "Content-Type: application/json" --data '{"type":"rtp", "data":{"rtpPort":5000,"rtcpPort":5001}}' -X POST http://127.0.0.1:34570/api/start`  
return:  
`{"status":"started"}`

- start rtmp streaming:
`curl -i -H "Accept: application/json" -H "Content-Type: application/json" --data '{"type":"uri", "data":{"encoder":"h264","bitrate":3000,"uri":"https://www.youtube.com/watch?v=CWu29PRCUvQ","stream_uri":"rtmp://rtmp.holacon.com/live/5ea28f442f59aa3a24198ae1","mode":"stream"}}' -X POST http://127.0.0.1:34570/api/start`  
return:  
`{"status":"started"}`  

- start rtmp streaming:
`curl -i -H "Accept: application/json" -H "Content-Type: application/json" --data '{"type":"uri", "data":{"encoder":"h264","bitrate":3000,"uri":"https://www.youtube.com/watch?v=5iPH-br_eJQ","stream_uri":"rtmp://rtmp.holacon.com/live/5ea28f442f59aa3a24198ae2","mode":"stream"}}' -X POST http://127.0.0.1:34570/api/start`  
return:  
`{"status":"started"}`  

- start rtmp streaming:
`curl -i -H "Accept: application/json" -H "Content-Type: application/json" --data '{"type":"uri", "data":{"encoder":"h264","bitrate":3000,"uri":"https://www.youtube.com/watch?v=rhFK5_Nx9xY","stream_uri":"rtmp://rtmp.holacon.com/live/5ea28f442f59aa3a24198ae3","mode":"stream"}}' -X POST http://127.0.0.1:34570/api/start`  
return:  
`{"status":"started"}`  

- start rtmp streaming:
`curl -i -H "Accept: application/json" -H "Content-Type: application/json" --data '{"type":"uri", "data":{"encoder":"h264","bitrate":2000,"uri":"https://www.youtube.com/watch?v=g45jXV-fg2E","stream_uri":"rtmp://rtmp.holacon.com/live/5ea28f442f59aa3a24198ae4","mode":"stream"}}' -X POST http://127.0.0.1:34570/api/start`  
return:  
`{"status":"started"}`  

- start html recordings:
`curl -i -H "Accept: application/json" -H "Content-Type: application/json" --data '{"type":"uri", "data":{"encoder":"h264","bitrate":2000,"uri":"https://www.youtube.com/watch?v=PX43GpT1MzA","stream_uri":"rtmp://rtmp.holacon.com/live/5ea28f442f59aa3a24198ae1","mode":"record"}}' -X POST http://127.0.0.1:34570/api/start`  
return:  
`{"status":"started"}`  

- start html recordings:
`curl -i -H "Accept: application/json" -H "Content-Type: application/json" --data '{"type":"uri", "data":{"encoder":"h264","bitrate":2000,"uri":"https://www.youtube.com/watch?v=9AiENE7uaO8","stream_uri":"rtmp://rtmp.holacon.com/live/5ea28f442f59aa3a24198ae2","mode":"record"}}' -X POST http://127.0.0.1:34570/api/start`  
return:  
`{"status":"started"}`  

- start html recordings:
`curl -i -H "Accept: application/json" -H "Content-Type: application/json" --data '{"type":"uri", "data":{"encoder":"h264","bitrate":2000,"uri":"https://www.youtube.com/watch?v=wnhvanMdx4s","stream_uri":"rtmp://rtmp.holacon.com/live/5ea28f442f59aa3a24198ae3","mode":"record"}}' -X POST http://127.0.0.1:34570/api/start`  
return:  
`{"status":"started"}`  

- start html recordings:
`curl -i -H "Accept: application/json" -H "Content-Type: application/json" --data '{"type":"uri", "data":{"encoder":"h264","bitrate":2000,"uri":"https://www.youtube.com/watch?v=g45jXV-fg2E","stream_uri":"rtmp://rtmp.holacon.com/live/5ea28f442f59aa3a24198ae4","mode":"record"}}' -X POST http://127.0.0.1:34570/api/start`  
return:  
`{"status":"started"}`  




- start holacon rtmp streaming:
`curl -i -H "Accept: application/json" -H "Content-Type: application/json" --data '{"type":"uri", "data":{"encoder":"h264","bitrate":2000,"uri":"https://livestream.holacon.com/?roomId=61fb7b0277eedd24fc7263de","stream_uri":"rtmp://rtmp.holacon.com/live/5ea28f442f59aa3a24198ae5","mode":"stream"}}' -X POST http://127.0.0.1:34570/api/start`  
return:  
`{"status":"started"}`  

  possible encoders value:  
  `"encoder":"h264"`  
  `"encoder":"vp8"`  

  
  possible mode value:  
  `"mode":"record"`  
  `"mode":"stream"`  
  `"mode":"both"`  
  

- stop recordings / rtmp streaming:  
`curl -i -H "Accept: application/json" -H "Content-Type: application/json" --data '{"id":1}' -X POST http://127.0.0.1:34570/api/stop`  
return:  
`{"status":"stopped"}`


- get recordings:  
`curl -i -H "Accept: application/json" -H "Content-Type: application/json" --data '{"id":1}' -X GET http://127.0.0.1:34570/api/get_records`  
return:  
`["/tmp/recorder_service/records/2020-09-04_13:19:15__0.webm", "/tmp/recorder_service/records/2020-09-04_14:29:15__1.webm"]`

