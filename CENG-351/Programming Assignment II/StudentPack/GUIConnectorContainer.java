import javax.swing.*;
import java.awt.*;
import java.util.HashMap;
import java.util.Map;

@SuppressWarnings("serial")
public class GUIConnectorContainer extends JComponent {	
    private Map<JComponent, JComponent> linked;

    public GUIConnectorContainer ()
    {
        super();
        
        linked = new HashMap<JComponent, JComponent>();
    }

    public void link (JComponent c1, JComponent c2)
    {
        linked.put(c1, c2);
        repaint();
    }

    protected void paintComponent(Graphics g)
    {
        Graphics2D g2d = (Graphics2D)g;
        g2d.setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
        g2d.setPaint(Color.green);
        g2d.setStroke(new BasicStroke(2));

        for (JComponent c1 : linked.keySet())
        {
			GUIHashRow castRow = (GUIHashRow) c1;
			GUIBucket castBucket = (GUIBucket) linked.get(c1);
			
			if(castRow.isVisited() && castBucket.isVisited())
			{
		        g2d.setPaint(Color.GREEN);
			}
			else
			{
		        g2d.setPaint(Color.BLACK);
			}
			
            Point p1 = getRectRight(getBoundsInWindow(c1));
            Point p2 = getRectLeft(getBoundsInWindow(linked.get(c1)));
            g2d.drawLine(p1.x + (p2.x - p1.x) / 10, p1.y, p2.x, p2.y);
        }
    }

    private Point getRectLeft(Rectangle rect)
    {
        return new Point(rect.x, rect.y + rect.height / 2);
    }
    
    private Point getRectRight(Rectangle rect)
    {
        return new Point(rect.x + rect.width, rect.y + rect.height / 2);
    }
    
    private Rectangle getBoundsInWindow(Component component)
    {
        return getRelativeBounds(component, getRootPaneAncestor(component));
    }

    private Rectangle getRelativeBounds(Component component, Component relativeTo)
    {
        return new Rectangle(getRelativeLocation(component, relativeTo), component.getSize());
    }

    private Point getRelativeLocation(Component component, Component relativeTo)
    {
        Point los = component.getLocationOnScreen();
        Point rt = relativeTo.getLocationOnScreen();
        return new Point(los.x - rt.x, los.y - rt.y);
    }

    private JRootPane getRootPaneAncestor (Component c)
    {
        for (Container p = c.getParent (); p != null; p = p.getParent ())
        {
            if (p instanceof JRootPane)
            {
                return ( JRootPane ) p;
            }
        }

        return null;
    }

    public boolean contains(int x, int y)
    {
        return false;
    }
}
