
var API_KEY = "ztnyKPLlQN7rc6VA7FolNxW5WQ4="

function getDeviceStatus(device_id) {
  var deviceConnected
  //查看设备连接状态，并刷新按钮状态
  wx.request({
    url: "https://api.heclouds.com/devices/" + device_id,
    header: {
      'content-type': 'application/x-www-form-urlencoded',
      "api-key": API_KEY
    },
    data: {

    },
    success(res) {
      console.log(res)
      if (res.data.data.online) {
        console.log("设备已经连接")
        deviceConnected = true
        return true
      } else {
        console.log("设备还未连接")
        deviceConnected = false
        return false
      }
    },
    fail(res) {
      console.log("请求失败")
      deviceConnected = false
      return false
    }
  })
}

function sendCmd1(id, data)
{
  var deviceConnected
  //查看设备连接状态，并刷新按钮状态
  wx.request({
    url: ("https://api.heclouds.com/cmds?device_id="+id+"/datapoints?datastream_id=pump_status"),
    header: {
      'content-type': 'application/json',
      "api-key": API_KEY
    },
    data: data,
    method: 'POST',
    success(res) {
      console.log(res)
    },
    fail(res) {
      console.log("请求失败")
      deviceConnected = false
    },
    complete() {
      if (deviceConnected) {
        console.log("complete ok")
        return true
      } else {
        console.log("complete error")
        return false
      }
    }
  })
}

function sendCmd(id, data)
{
  var deviceConnected
  //查看设备连接状态，并刷新按钮状态
  wx.request({
    url: ("https://api.heclouds.com/cmds?device_id="+id),
    header: {
      'content-type': 'application/json',
      "api-key": API_KEY
    },
    data: data,
    method: 'POST',
    success(res) {
      console.log(res)
    },
    fail(res) {
      console.log("请求失败")
      deviceConnected = false
    },
    complete() {
      if (deviceConnected) {
        console.log("complete ok")
        return true
      } else {
        console.log("complete error")
        return false
      }
    }
  })
}

function OneNet_Post(id){
  var that=this
 
  
  wx.request({
    url: "https://api.heclouds.com/cmds?device_id=" + id,
    method:'POST',
    header:{
      "content-type": 'application/json',
      "api-key": API_KEY
    },
    data:"pump_status:"+5,
    success(res){
      console.log("更新数据成功",res)
    },
    fail: function(res){
      wx.showToast({ title: '系统错误' })
    },
    complete:function(res){
      wx.hideLoading()
    }
  })
}


function getDataPoints(id) {
  var deviceConnected
  //查看设备连接状态，并刷新按钮状态
  wx.request({
    url: "https://api.heclouds.com/devices/" + id + "/datapoints?datastream_id=air_temperate",
    header: {
      'content-type': 'application/x-www-form-urlencoded',
      "api-key": API_KEY
    },
    data: {

    },
    success(res) {
      console.log(res)
      // if (res.data.data.online) {
      //   console.log("设备已经连接")
      //   deviceConnected = true
      // } else {
      //   console.log("设备还未连接")
      //   deviceConnected = false
      // }
    },
    fail(res) {
      console.log("请求失败")
      deviceConnected = false
    },
    complete() {
      if (deviceConnected) {
        console.log("complete ok")
        return true
      } else {
        console.log("complete error")
        return false
      }
    }
  })
}

module.exports = {
  getDeviceStatus,
  sendCmd,
  sendCmd1,
  getDataPoints
} 