//app.js
var onenet = require('js/onenet.js');

App({
  onLaunch: function () {
    // 展示本地存储能力
    var that = this;
    var logs = wx.getStorageSync('logs') || []
    logs.unshift(Date.now())
    wx.setStorageSync('logs', logs)
    
    //get devices of onenet
    wx.request({
      url: "https://api.heclouds.com/devices",
      header: {
        "api-key": "ztnyKPLlQN7rc6VA7FolNxW5WQ4="
      },
      data: {

      },
      success(res) {
        console.log("请求成功")
        console.log(res)
        var devices = [];
        if (res.data.errno == 0) {
          for (var i = 0; i < res.data.data.total_count; i++) {
            devices.push(res.data.data.devices[i]);
          }
        }
        that.globalData.devices = devices
        console.log(devices)
      },
      fail(res) {
        console.log("请求失败")
        // deviceConnected = false
      }
    })
  },

  globalData: {
    userInfo: null,
    devices: [],
    room_temperate_upper:32,
    room_temperate_lower:12,
    room_humidity_upper:90,
    room_humidity_lower:30,
    room_lux_upper:3000,
    // 白天1000-3000lux才能进行光合作用
    room_lux_lower:40,
    room_co2_upper:1000,
    room_co2_lower:100,
    soil_humidity_upper:100,
    soil_humidity_lower:20,
    soil_temperate_upper:50,
    soil_temperate_lower:-1,
    device_isonline:false,
  }
})