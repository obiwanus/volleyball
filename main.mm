#include <Cocoa/Cocoa.h>


@interface VolleyballAppDelegate : NSObject<NSApplicationDelegate>
@end


@implementation VolleyballAppDelegate

- (void) applicationDidFinishLaunching: (id) sender
{
}

- (BOOL) applicationShouldTerminateAfterLastWindowClosed: (NSApplication *) sender
{
    return YES;
}

- (void) applicationWillTerminate: (NSApplication *) sender
{
}

@end


void create_main_menu()
{
    NSMenu *menubar = [NSMenu new];
    NSMenuItem *app_menu_item = [NSMenuItem new];
    [menubar addItem:app_menu_item];

    [NSApp setMainMenu:menubar];

    NSMenu *app_menu = [NSMenu new];
    NSMenuItem *toggle_full_screen_menu_item = [
        [NSMenuItem alloc]
            initWithTitle:@"Toggle Full Screen"
            action:@selector(toggleFullScreen:)
            keyEquivalent:@"f"
        ];
    [app_menu addItem:toggle_full_screen_menu_item];

    NSMenuItem *quit_menu_item = [
        [NSMenuItem alloc]
            initWithTitle:@"Quit"
            action:@selector(terminate:)
            keyEquivalent:@"q"
        ];
    [app_menu addItem:quit_menu_item];

    [app_menu_item setSubmenu:app_menu];
}


int main(int argc, char *argv[])
{
    @autoreleasepool
    {
        NSApplication *app = [NSApplication sharedApplication];
        [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
        NSString *dir = [[NSFileManager defaultManager] currentDirectoryPath];
        NSLog(@"Working dir: %@", dir);

        create_main_menu();

        [app setDelegate:[[VolleyballAppDelegate alloc] init]];

        NSRect screen_rect = [[NSScreen mainScreen] frame];
        NSRect frame = NSMakeRect(
            (screen_rect.size.width - 960.0f) * 0.5,
            (screen_rect.size.height - 540.0f) * 0.5,
            960.0f,
            540.0f);

        NSWindow *window = [
            [NSWindow alloc]
                initWithContentRect:frame
                styleMask:NSTitledWindowMask
                        | NSClosableWindowMask
                        | NSMiniaturizableWindowMask
                        | NSResizableWindowMask
                backing:NSBackingStoreBuffered
                defer:NO
            ];

        [window setMinSize:NSMakeSize(100, 100)];
        [window setTitle:@"Volleyball"];
        [window makeKeyAndOrderFront:nil];

        [NSApp run];
    }
}
