var WxAutoImage = require('../../js/wxAutoImageCal.js');
var onenet = require('../../js/onenet.js');
var API_KEY = "ztnyKPLlQN7rc6VA7FolNxW5WQ4="
var app = getApp();

Page({
  data: {
    showModal: false,
    imgUrls: [
      '../../image/swiper1.jpg',
      '../../image/swiper1.jpg',
      '../../image/swiper1.jpg'
    ],
    room_name:"",
    input_name:"",
    air_temperate:0,
    air_humidity:0,
    air_lux:0,
    air_co2:0,
    soil_humidity:0,
    soil_temperate:0,
    air_temperate_time:"",
    air_humidity_time:"",
    air_lux_time:"",
    air_co2_time:"",
    soil_humidity_time:"",
    soil_temperate_time:"",
    air_temperate_check:0,
    air_humidity_check:0,
    air_lux_check:0,
    air_co2_check:0,
    soil_humidity_check:0,
    soil_temperate_check:0,
    id:0,
    index:0,
    myintervalid:0,
  },
  onLoad: function (e) {
    var that = this;
    console.log("onloading......");
    console.log("id is ", e.id)
    console.log("index is ", e.index)
    that.setData({
      id:e.id,
      index:e.index
    })
    that.getDataPoints(e.id)
    that.data.myintervalid = setInterval(function () {
      that.onShow()
    }, 3000)

    //get storage data
    var name = 'room_name_' + e.id
    try {
      var value = wx.getStorageSync(name)
      if (value) {
        // Do something with return value
        that.setData({
          room_name: value
        })
      }
    } catch (e) {
      // Do something when catch error
      console.log("get stroage data error!")
    }
  },
  onShow: function (e) {
    var that = this
    that.getDataPoints(that.data.id)
  },
  

  //onenet interfce
  getDataPoints: function (id) {
    var that = this
    var deviceConnected
    var air_temperate = 0
    var air_humidity = 0
    var air_lux = 0
    var air_co2 = 0
    var soil_humidity = 0
    var soil_temperate = 0
    var air_temperate_time = 0 
    var air_humidity_time = 0
    var air_lux_time = 0
    var air_co2_time = 0
    var soil_humidity_time = 0
    var soil_temperate_time = 0
    var air_temperate_check = 0
    var air_humidity_check = 0
    var air_lux_check = 0
    var air_co2_check = 0
    var soil_humidity_check = 0
    var soil_temperate_check = 0
    //查看设备连接状态，并刷新按钮状态
    wx.request({
      url: "https://api.heclouds.com/devices/" + id + "/datapoints?datastream_id=air_temperate,air_humidity,air_lux,air_co2,soil_humidity,soil_temperate,led_status",
      header: {
        'content-type': 'application/x-www-form-urlencoded',
        "api-key": API_KEY
      },
      data: {
      },
      success(res) {
        console.log(res)
        deviceConnected = true
        air_temperate = res.data.data.datastreams[0].datapoints[0].value
        air_temperate_time = res.data.data.datastreams[0].datapoints[0].at
        air_lux = res.data.data.datastreams[1].datapoints[0].value
        air_lux_time = res.data.data.datastreams[1].datapoints[0].at
        air_humidity = res.data.data.datastreams[2].datapoints[0].value
        air_humidity_time = res.data.data.datastreams[2].datapoints[0].at
        soil_humidity = res.data.data.datastreams[3].datapoints[0].value
        soil_humidity_time = res.data.data.datastreams[3].datapoints[0].at
        air_co2 = res.data.data.datastreams[5].datapoints[0].value
        air_co2_time = res.data.data.datastreams[5].datapoints[0].at
        soil_temperate = res.data.data.datastreams[6].datapoints[0].value
        soil_temperate_time = res.data.data.datastreams[6].datapoints[0].at
        console.log("air_temperate is ", air_temperate)
        console.log("air_temperate_time is ", air_temperate_time)
        console.log("air_humidity is ", air_humidity)
        console.log("air_lux is ", air_lux)
        console.log("air_co2 is ", air_co2)
        console.log("soil_humidity is ", soil_humidity)
        console.log("soil_temperate is ", soil_temperate)
        
        if(parseInt(air_temperate)>parseInt(app.globalData.room_temperate_upper) || parseInt(air_temperate)<parseInt(app.globalData.room_temperate_lower)){
          air_temperate_check = 0
        }else{
          air_temperate_check = 1
        }

        if(parseInt(air_humidity)>parseInt(app.globalData.room_humidity_upper) || parseInt(air_humidity)<parseInt(app.globalData.room_humidity_lower)){
          air_humidity_check = 0
        }else{
          air_humidity_check = 1
        }

        if(parseInt(air_lux)>parseInt(app.globalData.room_lux_upper) || parseInt(air_lux)<parseInt(app.globalData.room_lux_lower)){
          air_lux_check = 0
        }else{
          air_lux_check = 1
        }

        if(parseInt(air_co2)>parseInt(app.globalData.room_co2_upper) || parseInt(air_co2)<parseInt(app.globalData.room_co2_lower)){
          air_co2_check = 0
        }else{
          air_co2_check = 1
        }

        if(parseInt(soil_humidity)>parseInt(app.globalData.room_humidity_upper) || parseInt(soil_humidity)<parseInt(app.globalData.room_humidity_lower)){
          soil_humidity_check = 0
        }else{
          soil_humidity_check = 1
        }

        if(parseInt(soil_temperate)>parseInt(app.globalData.room_temperate_upper) || parseInt(soil_temperate)<parseInt(app.globalData.room_temperate_lower)){
          soil_temperate_check = 0
        }else{
          soil_temperate_check = 1
        }
        // console.log(parseInt(app.globalData.room_temperate_upper)+"------")
        that.setData({
          air_temperate_check: air_temperate_check,
          air_humidity_check: air_humidity_check,
          air_lux_check : air_lux_check,
          air_co2_check : air_co2_check,
          soil_humidity_check : soil_humidity_check,
          soil_temperate_check : soil_temperate_check,
          air_temperate : air_temperate,
          air_humidity : air_humidity,
          air_lux : air_lux,
          air_co2 : air_co2,
          soil_humidity : soil_humidity,
          soil_temperate : soil_temperate,
          air_temperate_time : air_temperate_time,
          air_humidity_time : air_humidity_time,
          air_lux_time : air_lux_time,
          air_co2_time : air_co2_time,
          soil_humidity_time : soil_humidity_time,
          soil_temperate_time : soil_temperate_time,
        })
      },
      fail(res) {
        console.log("请求失败")
        deviceConnected = false
      }
    })
  },

})