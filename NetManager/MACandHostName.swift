//
//  ScannerWithIP.swift
//  NetManager
//
//  Created by XWJACK on 2/8/16.
//  Copyright © 2016 XWJACK. All rights reserved.
//

import Foundation

// MARK: - It's very convenience to change IP to MACAddress or HostName
extension String {
    public var toMACAddress:String? {
        let back:UnsafeMutablePointer = UnsafeMutablePointer<Int8>((self as NSString).UTF8String)
        if ctoMACAddress(inet_addr(self), back) == -1 { return nil }
        return String.fromCString(back)
    }
    public var toHostName:String? {
        let back:UnsafeMutablePointer = UnsafeMutablePointer<Int8>((self as NSString).UTF8String)
        var ip = in_addr()
        guard inet_aton(back, &ip) == 1 else { return nil }
        let hp = gethostbyaddr(&ip, UInt32(sizeofValue(ip)), AF_INET)
        if hp != nil { return String.fromCString(hp.memory.h_name) }
        return nil
    }
}

