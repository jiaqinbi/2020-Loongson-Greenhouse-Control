// pages/data/rules/ruleview/ruleview.js
var app = getApp()

Page({

  /**
   * 页面的初始数据
   */
  data: {
    char_t : '<',
    room_temperate_upper:0,
    room_temperate_lower:100,
    room_humidity_upper:0,
    room_humidity_lower:100,
    room_lux_upper:0,
    room_lux_lower:100,
    room_co2_upper:0,
    room_co2_lower:100,
    soil_humidity_upper:0,
    soil_humidity_lower:100,
    soil_temperate_upper:0,
    soil_temperate_lower:100,
    
  },
  room_temperate_lower_input:function(options){
    this.setData({
      room_temperate_lower:options.detail.value.substr(0,options.detail.value.length-1)
    })
  },
  room_temperate_upper_input:function(options){
    this.setData({
      room_temperate_upper:options.detail.value.substr(0,options.detail.value.length-1)
    })
  },
  room_humidity_lower_input:function(options){
    this.setData({
      room_humidity_lower:options.detail.value.substr(0,options.detail.value.length-1)
    })
  },
  room_humidity_upper_input:function(options){
    this.setData({
      room_humidity_upper:options.detail.value.substr(0,options.detail.value.length-1)
    })
  },
  room_lux_lower_input:function(options){
    this.setData({
      room_lux_lower:options.detail.value.substr(0,options.detail.value.length-3)
    })
  },
  room_lux_upper_input:function(options){
    this.setData({
      room_lux_upper:options.detail.value.substr(0,options.detail.value.length-3)
    })
  },
  room_co2_lower_input:function(options){
    this.setData({
      room_co2_lower:options.detail.value.substr(0,options.detail.value.length-3)
    })
  },
  room_co2_upper_input:function(options){
    this.setData({
      room_co2_upper:options.detail.value.substr(0,options.detail.value.length-3)
    })
  },

  soil_humidity_lower_input:function(options){
    this.setData({
      soil_humidity_lower:options.detail.value.substr(0,options.detail.value.length-1)
    })
  },
  soil_humidity_upper_input:function(options){
    this.setData({
      soil_humidity_upper:options.detail.value.substr(0,options.detail.value.length-1)
    })
  },
  soil_temperate_lower_input:function(options){
    this.setData({
      soil_temperate_lower:options.detail.value.substr(0,options.detail.value.length-1)
    })
  },
  soil_temperate_upper_input:function(options){
    this.setData({
      soil_temperate_upper:options.detail.value.substr(0,options.detail.value.length-1)
    })
  },

  orderMeeting:function(options){
    app.globalData.room_temperate_lower=this.data.room_temperate_lower
    app.globalData.room_temperate_upper=this.data.room_temperate_upper
    app.globalData.room_humidity_lower=this.data.room_humidity_lower
    app.globalData.room_humidity_upper=this.data.room_humidity_upper
    app.globalData.room_lux_lower=this.data.room_lux_lower
    app.globalData.room_lux_upper=this.data.room_lux_upper
    app.globalData.room_co2_lower=this.data.room_co2_lower
    app.globalData.room_co2_upper=this.data.room_co2_upper
    app.globalData.soil_humidity_lower=this.data.soil_humidity_lower
    app.globalData.soil_humidity_upper=this.data.soil_humidity_upper
    app.globalData.soil_temperate_lower=this.data.soil_temperate_lower
    app.globalData.soil_temperate_upper=this.data.soil_temperate_upper
    
    wx.showModal({
      title: '提示',
      content: '是否确认修改？',
      success: function(res) {
        if (res.confirm) {
          // app.globalData.room_temperate_lower=this.data.room_temperate_lower
          // app.globalData.room_temperate_upper=this.data.room_temperate_upper
          // app.globalData.room_humidity_lower=this.data.room_humidity_lower
          // app.globalData.room_humidity_upper=this.data.room_humidity_upper
          // app.globalData.room_lux_lower=this.data.room_lux_lower
          // app.globalData.room_lux_upper=this.data.room_lux_upper
          // app.globalData.room_co2_lower=this.data.room_co2_lower
          // app.globalData.room_co2_upper=this.data.room_co2_upper
          // app.globalData.soil_humidity_lower=this.data.soil_humidity_lower
          // app.globalData.soil_humidity_upper=this.data.soil_humidity_upper
          // app.globalData.soil_temperate_lower=this.data.soil_temperate_lower
          // app.globalData.soil_temperate_upper=this.data.soil_temperate_upper
          wx.showToast({
            title:'修改成功',
            icon:'success',
            duration:2000
          })
       
        } else if (res.cancel) {
          console.log('用户点击取消')
        }
      }
    })
  },

  /**
   * 生命周期函数--监听页面加载
   */
  onLoad: function (options) {

    this.setData({
      room_temperate_upper:app.globalData.room_temperate_upper,
      room_humidity_upper:app.globalData.room_humidity_upper,
      room_lux_upper:app.globalData.room_lux_upper,
      room_co2_upper:app.globalData.room_co2_upper,
      soil_humidity_upper:app.globalData.soil_humidity_upper,
      soil_temperate_upper:app.globalData.soil_temperate_upper,
      room_temperate_lower:app.globalData.room_temperate_lower,
      room_humidity_lower:app.globalData.room_humidity_lower,
      room_lux_lower:app.globalData.room_lux_lower,
      room_co2_lower:app.globalData.room_co2_lower,
      soil_humidity_lower:app.globalData.soil_humidity_lower,
      soil_temperate_lower:app.globalData.soil_temperate_lower,
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
  onShow: function () {

  },

  /**
   * 生命周期函数--监听页面隐藏
   */
  onHide: function () {

  },

  /**
   * 生命周期函数--监听页面卸载
   */
  onUnload: function () {

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

  }
})