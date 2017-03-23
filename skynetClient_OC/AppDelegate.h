//
//  AppDelegate.h
//  skynetClient_OC
//
//  Created by mengyun on 2017/2/27.
//  Copyright © 2017年 mengyun. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <CoreData/CoreData.h>

@interface AppDelegate : UIResponder <UIApplicationDelegate>

@property (strong, nonatomic) UIWindow *window;

@property (readonly, strong) NSPersistentContainer *persistentContainer;

- (void)saveContext;


@end

