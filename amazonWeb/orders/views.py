from django.shortcuts import render
from .models import Order, OrderItem
from .forms import OrderCreateForm
from cart.cart import Cart
from django.contrib.auth.decorators import login_required
import socket
import xml.etree.ElementTree as ET
# from .tasks import order_created

def order_create(request):
    cart = Cart(request)
    item_id = []
    item_descriptions = []
    item_quantity = []
    if request.method == 'POST':
        form = OrderCreateForm(request.POST)
        if form.is_valid():
            form.instance.owner = request.user
            order = form.save()
            for item in cart:
                print(item['product'])
                print(item['quantity'])
                # Order.objects.create(owner = request.user,
                #                      addr_x = form.cleaned_data.get('addr_x'),
                #                      addr_y = form.cleaned_data.get('addr_y'), 
                #                      ups_id = form.cleaned_data.get('ups_id'),
                #                         product=item['product'],
                #                         price=item['price'],
                #                         product_cnt=item['quantity'],
                #                         )
                OrderItem.objects.create(order=order,
                                        product=item['product'],
                                        price=item['price'],
                                        quantity=item['quantity'])
                item_id.append(item['product'].id)
                item_descriptions.append(item['product'].description)
                item_quantity.append(item['quantity'])
            sendOrder(order, item_id, item_descriptions, item_quantity)    
            # clear the cart
            cart.clear()
            # launch asynchronous task
            #order_created.delay(order.id)
            
            return render(request,
                          'order/created.html',
                          {'order': order})
    else:
        form = OrderCreateForm()
    return render(request,
                  'order/create.html',
                  {'cart': cart, 'form': form})
    
@login_required
def viewOrder(request):
    order_list = Order.objects.filter(owner=request.user)
    context = {
        'order_list':order_list,
    }
    return render(request, 'order/orders.html', context)

# send order info to the server socket:
#  order: id, addr_x, addr_y, ups_id, product_description, id, quantity, userName
def sendOrder(order, item_id, item_descriptions, item_quantity):
    order_id = order.id
    addr_x = order.addr_x
    addr_y = order.addr_y
    ups_id = order.ups_id
    userName = order.owner.username
    root = ET.Element('order')
    items = ET.SubElement(root, 'items')
    
    for id, description, quantity in zip(item_id, item_descriptions, item_quantity):
        item_node = ET.SubElement(items, 'item')
        item_node.attrib={"id": str(id)}
        id_node = ET.SubElement(item_node, 'id')
        id_node.text = str(id)
        description_node = ET.SubElement(item_node, 'description')
        description_node.text = str(description)
        quantity_node = ET.SubElement(item_node, 'quantity')
        quantity_node.text = str(quantity)
    username_node = ET.SubElement(root, 'account_name')
    username_node.text = str(userName)
    order_id_node = ET.SubElement(root, 'order_id')
    order_id_node.text = str(order_id)
    addr_x_node = ET.SubElement(root, 'addr_x')
    addr_x_node.text = str(addr_x)
    addr_y_node = ET.SubElement(root, 'addr_y')
    addr_y_node.text = str(addr_y)
    ups_id_node = ET.SubElement(root, 'ups_id')
    ups_id_node.text = str(ups_id)
    
    xml_str = ET.tostring(root, encoding='utf-8', method='xml')   
    print(xml_str)
    
    HOST = '127.0.0.1'
    PORT = 8873
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        try:
            s.connect((HOST, PORT))
            print('connect to server success!')
            s.sendall(xml_str)
        except socket.error:
            print ("Couldn't connect with server or send xml")


