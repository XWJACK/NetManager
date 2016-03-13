//
//  Calculate.swift
//  NetManager
//
//  Created by XWJACK on 2/8/16.
//  Copyright © 2016 XWJACK. All rights reserved.
//

import Foundation

/// Calculate Network Information about all the possible IP address.....
public class Network{
    
    private var mask:in_addr_t
    private var ip:in_addr_t
    /**
    if ip or netmask is error,return nil
    
    - parameter ipAddress: ip
    - parameter netmask:   mask
    
    - returns: return nil if error
    */
    init?(ipAddress:String, netmask:String){
        ip = inet_addr(ipAddress)
        mask = inet_addr(netmask)
        
        guard ip != INADDR_NONE else { return nil }
        guard mask != INADDR_NONE else { return nil }
    }
    
    /// calculate host number in lan(contain 0 and 255)
    public var HostNumber:Int { return Int(~mask.bigEndian) + 1 }
    
    /// calculate first ip address like 192.168.1.0
    public var firstIPAddress:String { return String.fromCString(inet_ntoa(in_addr(s_addr: ip & mask)))! }
    
    /// calculate all host ip address like 192.168.1.1~192.168.1.254
    public var allHostIP:[String] {
        var temp = [String]()
        for i in 0..<HostNumber - 2 {
            let tempip = CFSwapInt32(CFSwapInt32(inet_addr(firstIPAddress)) + UInt32(i + 1))
            temp.append(String.fromCString(inet_ntoa(in_addr(s_addr: tempip)))!)
        }
        return temp
    }
}