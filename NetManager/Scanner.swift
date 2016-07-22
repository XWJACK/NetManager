//
//  Scanner.swift
//  NetManager
//
//  Created by XWJACK on 3/12/16.
//  Copyright © 2016 XWJACK. All rights reserved.
//

import UIKit

/// Globe varible to store all interface information: ip,mac,name,picture sort by ip.
var list = [String:[String]]()

public class Scanner {
    public static func refresh() {
        
        /// get local interface ip,netmask,broadcast
        let allInterfaceInformation = GetInterfaceInformation.getInterfaceInformation()
        
        if allInterfaceInformation["en0"] == nil {
            let alert = UIAlertView(title: "No Network", message: "You have not connected to Wi-Fi", delegate: nil, cancelButtonTitle: "OK")
            alert.show()
            return
        }
        let wifi = GetInterfaceInformation.getWIFIInformation()
        
        /// Calculate all ip address
        if let network = Network(ipAddress: allInterfaceInformation["en0"]![0], netmask: allInterfaceInformation["en0"]![1]) {
            let allhostip = network.allHostIP
            
            //let group = dispatch_group_create()
            dispatch_sync(dispatch_queue_create(nil, DISPATCH_QUEUE_SERIAL), {
                for i in allhostip {
                    dispatch_async(dispatch_queue_create(nil, DISPATCH_QUEUE_CONCURRENT),  {
                        let ping = xSocketPing(ipAddress: i, packetNumber: 1, timeout: 10, delegate: nil)
                        ping.xPing()
                    })
                }
            //dispatch_group_notify(group, dispatch_queue_create(nil, DISPATCH_QUEUE_SERIAL), {
                NSThread.sleepForTimeInterval(5)//这是个败笔
                
                for i in allhostip {
                    //if ip is myslef, jump.
                    if i == allInterfaceInformation["en0"]![0] { list[i] = ["xx:xx:xx:xx:xx:xx", UIDevice.currentDevice().name, "Local.png"]; continue }
                    
                    if let MACAddress = i.toMACAddress {
                        //if mac is route
                        
                        if wifi["BSSID"]! == MACAddress {
                            list[i] = [MACAddress, wifi["SSID"]!, "Wifi.png"]
                            continue
                        }
                        //others
                        if let hostname = i.toHostName { list[i] = [MACAddress, hostname, "User.png"]; continue }
                        list[i] = [MACAddress, "unknow", "User.png"]
                    }
                    //print(list)
                }
            })
            //print(allhostip)
        }else {
            let alert = UIAlertView(title: "Error", message: "Calculate Error", delegate: nil, cancelButtonTitle: "OK")
            alert.show()
            return
        }
        print(list)
    }
}
