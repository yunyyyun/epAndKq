//
//  ViewController.h
//  skynetClient_OC
//
//  Created by mengyun on 2017/2/27.
//  Copyright © 2017年 mengyun. All rights reserved.
//

#import <UIKit/UIKit.h>
#include "socket_server.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#undef __STDC_FORMAT_MACROS

@interface ViewController : UIViewController

- (int) socket_init;
//- (void *)_poll:(void *)ud;
@end

