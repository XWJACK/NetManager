//
//  xSocketPing.swift
//  NetManager
//
//  Created by 许文杰 on 1/7/16.
//  Copyright © 2016 许文杰. All rights reserved.
//

import Foundation

public class xSocketPing{
    
    private var ipAddress:String
    //default packet number 3 or you can setting it by yourself
    private var packetNumber:Int = 3
    private var timeout:Int = 1
    //refresh UI
    weak var delegate:refreshTextDelegate?

    init(ipAddress:String, delegate:refreshTextDelegate){
        self.ipAddress = ipAddress
        self.delegate = delegate
    }
    convenience init(ipAddress:String, packetNumber:Int, delegate:refreshTextDelegate){
        self.init(ipAddress: ipAddress,delegate: delegate)
        self.packetNumber = packetNumber
    }
    convenience init(ipAddress:String, packetNumber:Int, timeout:Int, delegate:refreshTextDelegate){
        self.init(ipAddress: ipAddress,packetNumber: packetNumber,delegate: delegate)
        self.timeout = timeout
    }
    
    public func xPing(){
        let tempIpAddress:UnsafeMutablePointer = UnsafeMutablePointer<Int8>((ipAddress as NSString).UTF8String)
        ipAddr = tempIpAddress
        sendPacketNumber = Int32(packetNumber)
        recvPacketNumber = Int32(packetNumber)
        
        settingIP()
        getPid()
        if createSocket() != -1 {
            settingSocket(Int32(timeout))
            let message:String = "PING \(ipAddress): 56 bytes of data.\n"
            refresh(message, speed: 0.0)
            let tempmessage:UnsafeMutablePointer = UnsafeMutablePointer<Int8>((message as NSString).UTF8String)
            var packetsequence:Int = 0
            var speed:Float = 0.0
            
            while packetsequence < packetNumber {
                if sendPacket(Int32(packetsequence)) != -1 {
                    receivePacket(tempmessage)
                    speed = (Float(packetNumber) - (Float(packetNumber) - Float(packetsequence))) / Float(packetNumber)
                    refresh(String(UTF8String: tempmessage)!, speed: speed)
                }
                sleep(1);
                packetsequence++
            }
            statistics(tempmessage)
            refresh(String(UTF8String: tempmessage)!, speed: 1)
            destorySocket()
        }
    }
    
    func refresh(text:String, speed:Float){
        delegate?.refresh(text, speed: speed)
    }
    deinit{
        print("xSocketPing destory")
    }
}