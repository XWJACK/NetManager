//
//  xSocketPing.swift
//  NetManager
//
//  Created by 许文杰 on 1/7/16.
//  Copyright © 2016 许文杰. All rights reserved.
//

import Foundation

public class xSocketPing{
    @asmname("ping") func Ping(ipAdderss ipAddress:UnsafePointer<UInt8>, number:Int, timeout:Int)
    
    private var ipAddress:String
    //default packet number 3 or you can setting it by yourself
    private var packetNumber:Int = 3
    private var timeout:Int = 1
    
    init(ipAddress:String){
        self.ipAddress = ipAddress
    }
    convenience init(ipAddress:String, packetNumber:Int){
        self.init(ipAddress: ipAddress)
        self.packetNumber = packetNumber
    }
    convenience init(ipAddress:String, packetNumber:Int, timeout:Int){
        self.init(ipAddress: ipAddress,packetNumber: packetNumber)
        self.timeout = timeout
    }
    
    public func xPing(){
        Ping(ipAdderss: ipAddress, number: packetNumber,timeout: timeout)
    }
    
    deinit{
        print("xSocketPing destory")
    }
}