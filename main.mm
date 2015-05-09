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
{
    uint8 *offscreenBuffer;
    int Width;
    int Height;
    CGDataProviderRef provider;
    CGImageRef image;
    CGContextRef context;
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

    CGDataProviderRelease(provider);

    [super dealloc];
}

- (void) toggleFullScreen: (id)sender
{
    printf("Toggle full screen\n");
}

- (BOOL) acceptsFirstResponder
{
    return YES;
}

- (void) drawRectangle: (NSRect) rect
{
    NSLog(@"Draw at: %ld", (long) offscreenBuffer);
    int i = 0;
    for (i = rect.origin.y;
         i < Height && i < (rect.origin.y + rect.size.height); i++)
    {
        uint32 *Row = (uint32 *) offscreenBuffer + i * Width;
        for (int j = rect.origin.x;
             j < Width && j < rect.origin.x + rect.size.width; j++)
        {
            Row[j] = 0x44444400;
        }
    }
}

- (void) mouseDown: (NSEvent *) event
{
    NSRect rect;
    NSPoint clickLocation;

    // convert the mouse-down location into the view coords
    clickLocation = [self convertPoint:[event locationInWindow]
                          fromView:nil];

    rect.origin = clickLocation;
    rect.size.width = 40.0;
    rect.size.height = 40.0;

    [self drawRectangle:rect];
}

- (void) displayBitmap
{
    // Displays the contents of the offscreen buffer in the view

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
    provider = CGDataProviderCreateWithData(NULL, offscreenBuffer,
                                            BufferLength, NULL);

    CGColorSpaceRef colorspace = CGColorSpaceCreateDeviceRGB();
    image = CGImageCreate(
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

    context = (CGContextRef)[[NSGraphicsContext currentContext] graphicsPort];
    CGContextDrawImage(context, self.bounds, image);


    // NSBitmapImageRep *ImageRep = [
    //     [NSBitmapImageRep alloc]
    //         initWithBitmapDataPlanes:&offscreenBuffer
    //         pixelsWide:Width
    //         pixelsHigh:Height
    //         bitsPerSample:8
    //         samplesPerPixel:BytesPerPixel
    //         hasAlpha:YES
    //         isPlanar:NO
    //         colorSpaceName:NSDeviceRGBColorSpace
    //         bytesPerRow:(Width * BytesPerPixel)
    //         bitsPerPixel:(BytesPerPixel * 8)
    // ];

    // CGImage *Image = [[CGImage alloc] init];
    // [Image addRepresentation:ImageRep];

    // [self setImage:Image];
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
