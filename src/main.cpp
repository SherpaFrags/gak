#include "html_parser.h"
#include "renderer.h"
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>

int main() {
    // Register custom-alert if it hasn't been merged into TAG_MODIFIERS yet
    TAG_MODIFIERS["custom-alert"] = [](Element e, const AttributeMap&) {
        return e | borderLight | color(Color::Yellow) | bgcolor(Color::Red);
    };

    // Cleaned up to use purely tags and semantic elements—no inline CSS styles!
    std::string advanced_html = R"(
<div>
    <h1>Heading Level 1 (Red)</h1>
    <h2>Heading Level 2 (Blue)</h2>
    <h3>Heading Level 3 (Cyan)</h3>
    <h4>Heading Level 4 (Green)</h4>
    <h5>Heading Level 5 (Yellow)</h5>
    <h6>Heading Level 6 (Magenta)</h6>
    
    <hr>

    <p>
        This paragraph tests text formatting like <b>bold text</b>, 
        <strong>strong text</strong>, <i>italics/dimmed text</i>, and 
        <em>emphasized text</em>. You can also see <del>deleted/strikethrough text</del>, 
        <s>struck text</s>, <ins>inserted text</ins>, and <u>underlined elements</u>.
    </p>
    <p>
        Here is some <small>small/dim text</small> next to a highlighted <mark>marked text block</mark>.
    </p>

    <hr>

    <p>
        Here is a valid anchor link: <a href="https://github.com">Visit GitHub</a>. 
        And here is an anchor with no href attribute: <a>Anchor with no target</a>.
    </p>
    
    <p>
        Press <kbd>Ctrl</kbd> + <kbd>C</kbd> to exit the application. 
        The compiler output uses <code>std::cout</code> for logging data. 
        System response template: <samp>SUCCESS: 200 OK</samp>
    </p>

    <pre>
// This is a block layout pre-formatted text box
void setup() {
    init_renderer();
}
    </pre>

    <hr>

    <blockquote>
        This is a semantic blockquote. The engine should automatically prefix 
        this block layout with a vertical pipe border indicator and dim its contents.
    </blockquote>

    <fieldset legend="System Configuration">
        <span>
            <b>Engine Core Status: </b>
            <strong>Active and Listening</strong>
        </span>
    </fieldset>

    <hr>

    <p>Unordered List Test:</p>
    <ul>
        <li>First unordered element row item</li>
        <li>Second unordered element row item</li>
    </ul>

    <p>Ordered List Test:</p>
    <ol>
        <li>First ordered sequence step</li>
        <li>Second ordered sequence step</li>
    </ol>

    <hr>

    <span>
        <button>Active Button</button>
        <button disabled="true">Disabled Button</button>
    </span>

    <p>
        Form Input Elements:
        <input type="text" value="Text Field Area">
        <input type="checkbox" checked="true"> Checkbox Variant
    </p>
    
    <textarea>This is text inside a standalone textarea boundary layout box.</textarea>

    <hr>

    <alert-success>SUCCESS: DOM Tree parsed completely without layout calculation errors.</alert-success>
    <alert-warning>WARNING: High memory usage threshold observed on node allocation maps.</alert-warning>
    <alert-danger>CRITICAL ERROR: Terminal viewport size dropped below minimum threshold boundary.</alert-danger>
</div>
)";

    size_t pos = 0;
    auto parsed_dom = parse_html_to_dom(advanced_html, pos);

    auto my_renderer = Renderer([&]() {
        return compile_dom_to_ftxui(parsed_dom) | border;
    });

    auto screen = ScreenInteractive::TerminalOutput();
    screen.Loop(my_renderer);

    return 0;
}