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


@interface VolleyballView : NSImageView
@end


@implementation VolleyballView

- (id) init
{
    self = [super init];

    if (self == nil)
    {
        return nil;
    }

    return self;
}

- (void) toggleFullScreen: (id)sender
{
    printf("Toggle full screen\n");
}

- (BOOL) acceptsFirstResponder
{
    return YES;
}

// - (void) mouseDragged: (NSEvent *) event
// {
//     NSRect rect;
//     NSPoint clickLocation;

//     // convert the mouse-down location into the view coords
//     clickLocation = [self convertPoint:[event locationInWindow]
//                           fromView:nil];

//     rect.origin = clickLocation;
//     rect.size.width = 10.0;
//     rect.size.height = 10.0;

//     [[NSColor redColor] set];
//     [NSBezierPath fillRect:rect];

//     [self displayRect:rect];
// }

- (void) setUp
{
    int Width = self.bounds.size.width;
    int Height = self.bounds.size.height;
    int BytesPerPixel = 4;

    unsigned char *offscreenBuffer = (unsigned char *) malloc(Width * Height * BytesPerPixel);

    // Fill the buffer
    for (int i = 0; i < Height; i++)
    {
        uint32 *Row = (uint32 *) (offscreenBuffer + i * BytesPerPixel * Width);
        for (int j = 0; j < Width; j++)
        {
            Row[j] = 0xAAFFAAFF;
        }
    }

    NSBitmapImageRep *ImageRep = [
        [NSBitmapImageRep alloc]
            initWithBitmapDataPlanes:&offscreenBuffer
            pixelsWide:Width
            pixelsHigh:Height
            bitsPerSample:8
            samplesPerPixel:BytesPerPixel
            hasAlpha:YES
            isPlanar:NO
            colorSpaceName:NSCalibratedRGBColorSpace
            bytesPerRow:(Width * BytesPerPixel)
            bitsPerPixel:(BytesPerPixel * 8)
    ];

    NSImage *Image = [[NSImage alloc] init];
    [Image addRepresentation:ImageRep];

    [self setImage:Image];
}

- (void) mouseUp: (NSEvent *) event
{

}

- (BOOL) isOpaque
{
    return YES;
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

        VolleyballView* view = [[VolleyballView alloc] init];
        [view setFrame:[[window contentView] bounds]];
        [view setUp];
        [view setAutoresizingMask:NSViewWidthSizable | NSViewHeightSizable];

        [[window contentView] addSubview:view];
        [window setMinSize:NSMakeSize(100, 100)];
        [window setTitle:@"Volleyball"];
        [window makeKeyAndOrderFront:nil];

        [NSApp run];
    }
}
