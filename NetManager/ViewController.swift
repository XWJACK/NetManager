//
//  ViewController.swift
//  NetManager
//
//  Created by 许文杰 on 1/7/16.
//  Copyright © 2016 许文杰. All rights reserved.
//

import UIKit

class ViewController: UIViewController {

    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view, typically from a nib.
        let ping = xping(addr: "192.168.1.1")
        ping.ping()
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }
}

