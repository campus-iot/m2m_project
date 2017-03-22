FROM nodered/node-red-docker:latest
RUN npm install node-red-contrib-influxdb
COPY flows.json /data/
CMD ["npm", "start", "--", "--userDir", "/data"]
EXPOSE 8123/udp