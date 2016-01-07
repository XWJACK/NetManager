//
//  xSocketPing.swift
//  NetManager
//
//  Created by 许文杰 on 1/7/16.
//  Copyright © 2016 许文杰. All rights reserved.
//

import Foundation

@asmname("ping") func xSocketPing(ipAddress:UnsafePointer<Int8>, number:Int32)

public class xping{
    var addr:String
    init(addr:String){
        self.addr = addr
    }
    public func ping(){
        xSocketPing(addr, number: 3)
    }
}