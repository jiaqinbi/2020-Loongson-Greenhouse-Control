var WxAutoImage = require('../../js/wxAutoImageCal.js');

var app = getApp();

Page({
  data: {
    imgUrls: [
      '../../image/sochina1.png',
      '../../image/sochina2.png',
      '../../image/sochina3.png'
    ],
    indicatorDots: true,
    vertical: false,
    autoplay: true,
    interval: 3000,
    duration: 1200,
    devices: [],
    bindDisabled:false
  },

  onLoad: function (e){
    var that = this;
    var name = ''
    var devices_origin = ''
    console.log("onloading......");
    

    // 从app页面的devicesList中获取设备列表
    devices_origin = app.globalData.devices
    console.log(e);

    for (var i = 0; i < devices_origin.length; i++) {
      name = 'room_name_' + devices_origin[i].id
      
      try {
        var value = wx.getStorageSync(name)
        if (value) {
          devices_origin[i].room_name = value;
        }
      } catch (e) {
        console.log("get stroage data error!")
      }
    }

    that.setData({
      devices : devices_origin,
    })

  },

  onShow: function (e) {
    var that = this
    var name = ''
    var devices_origin = ''

    // 从app页面的devicesList中获取设备列表
    devices_origin = app.globalData.devices
    for (var i = 0; i < devices_origin.length; i++) {
      name = 'room_name_' + devices_origin[i].id
      try {
        var value = wx.getStorageSync(name)
        if (value) {
          devices_origin[i].room_name = value;
        }
      } catch (e) {
        console.log("get stroage data error!")
      }
    }

    that.setData({
      devices: devices_origin
    })
  },
})