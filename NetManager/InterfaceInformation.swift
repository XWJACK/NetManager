//
//  wifi.swift
//  Test
//
//  Created by XWJACK on 1/14/16.
//  Copyright © 2016 XWJACK. All rights reserved.
//

import Foundation

import NetworkExtension
import SystemConfiguration.CaptiveNetwork


/*class wifi {
    static func getIPAddresses() -> [String] {
        var addresses = [String]()
        
        // Get list of all interfaces on the local machine:
        var ifaddr : UnsafeMutablePointer<ifaddrs> = nil
        
        //The getifaddrs() function returns the value 0 if successful; otherwise the value -1 is returned and the global variable errno is set to indicate the error.
        if getifaddrs(&ifaddr) == 0 {
            
            // For each interface ...
            for (var ptr = ifaddr; ptr != nil; ptr = ptr.memory.ifa_next) {
                let flags = Int32(ptr.memory.ifa_flags)
                var addr = ptr.memory.ifa_addr.memory
                
                // Check for running IPv4, IPv6 interfaces. Skip the loopback interface.
                if (flags & (IFF_UP|IFF_RUNNING|IFF_LOOPBACK)) == (IFF_UP|IFF_RUNNING) {/* interface is up *//* resources allocated */
                    if addr.sa_family == UInt8(AF_INET) || addr.sa_family == UInt8(AF_INET6) {
                        
                        // Convert interface address to a human readable string:
                        var hostname = [CChar](count: Int(NI_MAXHOST), repeatedValue: 0)
                        if (getnameinfo(&addr, socklen_t(addr.sa_len), &hostname, socklen_t(hostname.count),nil, socklen_t(0), NI_NUMERICHOST) == 0) {
                            if let address = String.fromCString(hostname) {
                                addresses.append(address)
                            }
                        }
                    }
                }
            }
            freeifaddrs(ifaddr)
        }
        return addresses
    }
}*/


class GetInterfaceInformation{
    
    /**
    get ethernet information about name,address,netmask,broadcast
    
    - returns: return Dictionary contain Ethernet name,ip address,netmask,broadcast
    */
    static func getInterfaceInformation() -> [String:[String]] {
        var information = [String:[String]]()
        
        var ifaddr:UnsafeMutablePointer<ifaddrs> = nil
        //retrieve the current interface -- return 0 on success
        if getifaddrs(&ifaddr) == 0 {
            var interface = ifaddr
            //loop through linked list of interface
            while interface != nil {
                if interface.memory.ifa_addr.memory.sa_family == UInt8(AF_INET) {//ipv4
                    let interfaceName = String.fromCString(interface.memory.ifa_name)
                    let interfaceAddress = String.fromCString(inet_ntoa(UnsafeMutablePointer<sockaddr_in>(interface.memory.ifa_addr).memory.sin_addr))
                    let interfaceNetmask = String.fromCString(inet_ntoa(UnsafeMutablePointer<sockaddr_in>(interface.memory.ifa_netmask).memory.sin_addr))
                    //ifa_dstaddr /* P2P interface destination */
                    //The ifa_dstaddr field references the destination address on a P2P inter-face, interface,
                    //face, if one exists, otherwise it contains the broadcast address.
                    let interfaceBroadcast = String.fromCString(inet_ntoa(UnsafeMutablePointer<sockaddr_in>(interface.memory.ifa_dstaddr).memory.sin_addr))
                    
                    if let name = interfaceName {
                        information[name] = [interfaceAddress!,interfaceNetmask!,interfaceBroadcast!]
                    }
                }
                interface = interface.memory.ifa_next
            }
            freeifaddrs(ifaddr)
        }
        return information
    }
    
    /**
    get WI-FI information
    
    - returns: return Dictionary contain ssid and bssid
    */
    static func getWIFIInformation() -> [String:String] {
        var informationDictionary = [String:String]()
        if #available(iOS 9.0, *) {
            let information = NEHotspotHelper.supportedNetworkInterfaces()
            informationDictionary["SSID"] = information[0].SSID!
            informationDictionary["BSSID"] = information[0].BSSID!
            return informationDictionary
        } else {
            // Fallback on earlier versions
            let informationArray:NSArray? = CNCopySupportedInterfaces()
            if let information = informationArray {
                let dict:NSDictionary? = CNCopyCurrentNetworkInfo(information[0] as! CFStringRef)
                if let temp = dict {
                    informationDictionary["SSID"] = String(temp["SSID"]!)
                    informationDictionary["BSSID"] = String(temp["BSSID"]!)
                    return informationDictionary
                }
            }
        }
        return informationDictionary
    }
}


