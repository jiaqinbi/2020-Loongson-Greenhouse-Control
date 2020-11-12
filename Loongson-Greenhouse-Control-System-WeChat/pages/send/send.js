var WxAutoImage = require('../../js/wxAutoImageCal.js');
var onenet = require('../../js/onenet.js');
var API_KEY = "ztnyKPLlQN7rc6VA7FolNxW5WQ4="

var app = getApp();

// pages/send/send.js
Page({

  /**
   * 页面的初始数据
   */
  data: {
    show: false,  //控制下拉列表的显示和隐藏
    selectData:['1','2','3','4','5','6'],//下拉列表的数据
    first_click:0,
    index:0,//选择的下拉列表下标
    devices: [],
    device_id:['637181459',''],
    isonline:0,
    employId: '',
    bindDisabled:false,
    myintervalid:0,
    deviceConnected:0,
    ledStatusSwitchChecked:false,
    pumpStatusSwitchChecked:false,
    lightStatusSwitchChecked:false,

  },

  // 点击下拉显示框
 selectTap(){
  var that = this;
  var devices_origin = ''
  var isonline = 0  

  devices_origin = app.globalData.devices

  app.globalData.device_isonline = isonline
  that.setData({
    devices : devices_origin,
    show: !this.data.show,
  });
  },

  // 点击下拉列表
  optionTap(e){
    let Index=e.currentTarget.dataset.index+1;//获取点击的下拉列表的下标
    var that = this;
    var first_click = 0;
    var devices_origin = ''
    devices_origin = app.globalData.devices
    console.log(that.data.device_id[0]+"---")
    if(devices_origin.length != 0){
      first_click = 1
    }else{
      first_click = 0;
    }
    that.setData({
      devices : devices_origin,
      index:Index,
      first_click: first_click,
      show:!this.data.show
    });
  },

  /**
   * 生命周期函数--监听页面加载
   */
  onLoad: function (options) {
    var that = this;
    var devices_origin = ''
    var isonline = 0
    var deviceConnected = false
    devices_origin = app.globalData.devices
    wx.request({
      url: "https://api.heclouds.com/devices",
      header: {
        "api-key": "ztnyKPLlQN7rc6VA7FolNxW5WQ4="
      },
      data: {

      },
      success(res) {
        isonline = res.data.data.devices[0].online
        
        that.setData({
          deviceStatusSwitchChecked:isonline,
          deviceConnected: isonline,
          isonline:isonline,
          baiyexiangStatusSwitchChecked:isonline
        })
      },
      fail(res) {
        console.log("请求失败")
        // deviceConnected = false
      }
    })

    that.getDataPoints(options.id)
    that.data.myintervalid = setInterval(function(){
      that.onShow()
    }, 3000)

    that.setData({
      devices : devices_origin,
    })
  },

  /**
   * 生命周期函数--监听页面初次渲染完成
   */
  onReady: function () {

  },

  /**
   * 生命周期函数--监听页面显示
   */
  onShow: function (options) {
    var that = this
    var name = ''
    var devices_origin = ''
    
    // 从app页面的devicesList中获取设备列表
    devices_origin = app.globalData.devices
    that.getDataPoints(that.data.id)
    that.setData({
      devices: devices_origin
    })
  },



  /**
   * 生命周期函数--监听页面隐藏
   */
  onHide: function () {
    clearInterval(this.data.myintervalid);
  },

  /**
   * 生命周期函数--监听页面卸载
   */
  onUnload: function () {
    clearInterval(this.data.myintervalid);
  },

  /**
   * 页面相关事件处理函数--监听用户下拉动作
   */
  onPullDownRefresh: function () {

  },

  /**
   * 页面上拉触底事件的处理函数
   */
  onReachBottom: function () {

  },

  /**
   * 用户点击右上角分享
   */
  onShareAppMessage: function () {

  },

  //onenet interfce
  getDataPoints: function (id) {
    var that = this
   
    var led_status = 0
    var light_status = 0
    var pump_status = 0
    //查看设备连接状态，并刷新按钮状态
    wx.request({
      url: "https://api.heclouds.com/devices/"+that.data.device_id[0]+"/datapoints?datastream_id=led_status,light_status,pump_status",
      header: {
        'content-type': 'application/x-www-form-urlencoded',
        "api-key": API_KEY
      },
      data: {

      },
      success(res) {
        that.setData({
          // deviceConnected: deviceConnected,
          // isonline:isonline
        })
        if (that.data.switchFlag != true)
        {
          //console("res:"+res)
          led_status = res.data.data.datastreams[1].datapoints[0].value
          light_status = res.data.data.datastreams[0].datapoints[0].value
          pump_status = res.data.data.datastreams[2].datapoints[0].value
          console.log("pump_status:"+pump_status)
          switch (parseInt(led_status)) {
            case 4:
              that.setData({
                pumpStatusSwitchChecked: true,
              })
              break;
            case 5:
              that.setData({
                pumpStatusSwitchChecked: false,
              })
              break;
          }

          switch (parseInt(light_status)) {
            case 2:
              that.setData({
                lightStatusSwitchChecked: true,
              })
              break;
            case 3:
              that.setData({
                lightStatusSwitchChecked: false,
              })
              break;
          }
        }else{
          that.setData({
            switchFlag:false
          })
        }
      },
      fail(res) {
        console.log("请求失败")
        // deviceConnected = false
      }
    })
  },
 
  btn_light_status_fun: function (options) {
    var that = this;
    console.log("id:"+that.data.device_id[0])
    console.log("options.detail.value:"+options.detail.value)
    if (true == options.detail.value)
    {
      console.log("ready to open light!");
      onenet.sendCmd(that.data.device_id[0], "2")
      that.setData({
        lightStatusSwitchChecked: true
      })
    }else
    {
      console.log("ready to close light!");
      onenet.sendCmd(that.data.device_id[0], "3")
      that.setData({
        lightStatusSwitchChecked: false,
        switchFlag:true
      })
    }
  },


  btn_pump_status_fun: function (options) {
    var that = this;
    console.log("id:"+that.data.device_id[0])
    console.log("options.detail.value:"+options.detail.value)
    if (true == options.detail.value)
    {
      console.log("ready to open pump!");
      onenet.sendCmd(that.data.device_id[0], "4")
      that.setData({
        pumpStatusSwitchChecked: true
      })
    }else
    {
      console.log("ready to close pump!");
      onenet.sendCmd(that.data.device_id[0], "5")
      that.setData({
        pumpStatusSwitchChecked: false,
        switchFlag:true
      })
    }
  },

})