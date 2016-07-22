//
//  ViewController.swift
//  NetManager
//
//  Created by XWJACK on 1/7/16.
//  Copyright © 2016 XWJACK. All rights reserved.
//

import UIKit

protocol refreshTextDelegate:NSObjectProtocol{
    func refresh(text:String, speed:Float)
}

class ViewController: UIViewController,refreshTextDelegate {

    @IBOutlet weak var xprogress: UIProgressView!
    @IBOutlet weak var xip: UITextField!
    @IBOutlet weak var xnumber: UITextField!
    @IBOutlet weak var xtext: UITextView!
    
    var number:Int = 3
    var ip:String = "192.168.1.1"
    
    override func viewDidLoad() {
        super.viewDidLoad()
        // Do any additional setup after loading the view, typically from a nib.
        xtext.text = ""
    }

    override func didReceiveMemoryWarning() {
        super.didReceiveMemoryWarning()
        // Dispose of any resources that can be recreated.
    }
    
    @IBAction func xNumberChange(sender: UIStepper) {
        xnumber.text = "\(Int(sender.value))"
        self.number = Int(sender.value)
    }
    
    @IBAction func beginPing(sender: UIButton) {
//        sender.enabled = false
//        xip.enabled = false
//        xnumber.enabled = false
//        
//        xprogress.progress = 0
//        xtext.text = ""
//        
//        let ip = xip.text
//        if ip == ""{
//            let alert = UIAlertView(title: "Error", message: "No IP Address", delegate: self, cancelButtonTitle: "OK")
//            alert.show()
//        }else{
//            dispatch_async(dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0), {
//                let ping:xSocketPing = xSocketPing(ipAddress: "\(ip!)", packetNumber: self.number, delegate: self)
//                ping.xPing()
//            })
//        }
//        xnumber.enabled = true
//        xip.enabled = true
//        sender.enabled = true
        
    }
    
    
    func refresh(text:String, speed:Float) {
        dispatch_async(dispatch_get_main_queue(), {
            //更新进度条
            self.xprogress.progress = speed
            //更新UITextView,追加内容并滚动到最下面
            self.xtext.text.appendContentsOf(text)
            self.xtext.scrollRangeToVisible(NSMakeRange(self.xtext.text.characters.count, 0))
        })
    }
    override func touchesBegan(touches: Set<UITouch>, withEvent event: UIEvent?) {
        self.view.endEditing(true)
    }
}

