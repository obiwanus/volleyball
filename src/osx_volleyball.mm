/* NOTE: This is an unfinished OS X layer which is not used at the moment */

#include <Cocoa/Cocoa.h>
#include <OpenGL/gl.h>


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
{
    uint8 *offscreenBuffer;
    int Width;
    int Height;
    CGImage *Image;
}
@end


@implementation VolleyballView

- (id) init
{
    self = [super init];

    if (self == nil)
    {
        return nil;
    }

    offscreenBuffer = NULL;  // not allocated yet
    Width = 0;
    Height = 0;

    return self;
}

- (void) dealloc
{
    if (offscreenBuffer)
    {
        free(offscreenBuffer);
    }

    [super dealloc];
}

- (BOOL) acceptsFirstResponder
{
    return YES;
}

- (BOOL) isFlipped
{
    return YES;
}

- (void) drawRect: (NSRect) rect
{
    NSLog(@"draw rect: %f x %f", rect.size.width, rect.size.height);
    // [Image drawAtPoint:NSZeroPoint fromRect:NSZeroRect operation:NSCompositeSourceOver fraction:1];
}

- (void) setUp
{
    Width = self.bounds.size.width;
    Height = self.bounds.size.height;
    int BytesPerPixel = 4;
    int BufferLength = Width * Height * BytesPerPixel;

    offscreenBuffer = (uint8 *) malloc(BufferLength);

    // Fill the buffer
    for (int i = 0; i < Height; i++)
    {
        uint32 *Row = (uint32 *) offscreenBuffer + i * Width;
        for (int j = 0; j < Width; j++)
        {
            uint8 Blue = (uint8) j;
            uint8 Green = (uint8) i;

            Row[j] = ((Green << 16) | Blue << 8);
        }
    }

    // Create a CGImage with the pixel data
    CGDataProviderRef provider = CGDataProviderCreateWithData(NULL, offscreenBuffer,
                                                           BufferLength, NULL);

    CGColorSpaceRef colorspace = CGColorSpaceCreateDeviceRGB();
    Image = CGImageCreate(
        Width,
        Height,
        8,      // bits per component
        32,     // bits per pixel
        Width * 4,  // bytes per row
        colorspace,
        kCGBitmapByteOrder32Big,  // bitmap info
        provider,
        NULL,   // decode array
        true,   // should interpolate
        kCGRenderingIntentDefault   // rendering intent
    );

    CGColorSpaceRelease(colorspace);
    // CGDataProviderRelease(provider);

    CGContextRef context = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
    CGContextDrawImage(context, self.bounds, Image);

    [self display];
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
